# Internal port of the small subset of 'decor' functions that cpp4r needs.
# This removes the run-time 'decor' dependency and adds support for capturing
# the C++ namespace of a decorated function.

cpp_files <- function(pkg = ".") {
  if (length(pkg) == 0 || !nzchar(pkg[[1L]])) {
    return(character())
  }

  src <- file.path(pkg, "src")
  if (!dir.exists(src)) {
    return(character())
  }

  out <- list.files(src, full.names = TRUE, pattern = "[.](cc|cpp|h|hpp)$")
  # stable, locale-independent order
  out[order(out, method = "radix")]
}

empty_decorations <- function() {
  list(
    file = character(),
    line = integer(),
    decoration = character(),
    namespace = character(),
    params = list(),
    context = list()
  )
}

empty_args <- function() {
  list(type = character(), name = character(), default = character())
}

cpp_decorations <- function(pkg = ".", files = cpp_files(pkg = pkg), is_attribute = FALSE) {
  res <- lapply(files, function(file) {
    if (!file.exists(file)) {
      return(empty_decorations())
    }

    if (is_attribute) {
      raw <- readChar(file, file.size(file), useBytes = FALSE)
      if (!nzchar(raw)) {
        return(empty_decorations())
      }
      without_comments <- blank_comments(raw)
      lines <- strsplit(without_comments, "\r?\n")[[1]]
    } else {
      lines <- readLines(file, warn = FALSE)
    }

    pattern <- cpp_attribute_pattern(is_attribute)
    start <- grep(pattern, lines)
    if (!length(start)) {
      return(empty_decorations())
    }

    end <- c(utils::tail(start, -1L) - 1L, length(lines))

    text <- lines[start]
    content <- sub(paste0(pattern, ".*"), "\\1", text)
    decoration <- sub("\\(.*$", "", content)

    has_args <- grepl("\\(", content)
    params <- vector("list", length(content))
    for (i in seq_along(content)) {
      if (has_args[[i]]) {
        params[[i]] <- as.list(parse(text = content[[i]])[[1]][-1])
      }
    }

    namespaces <- namespace_per_line(lines)[start]

    context <- mapply(function(.x, .y) lines[seq(.x, .y)], start, end, SIMPLIFY = FALSE)

    list(
      file = rep(file, length(start)),
      line = start,
      decoration = decoration,
      namespace = namespaces,
      params = params,
      context = context
    )
  })

  if (length(res) == 0) {
    return(empty_decorations())
  }
  list(
    file = unlist(lapply(res, `[[`, "file")),
    line = unlist(lapply(res, `[[`, "line")),
    decoration = unlist(lapply(res, `[[`, "decoration")),
    namespace = unlist(lapply(res, `[[`, "namespace")),
    params = unlist(lapply(res, `[[`, "params"), recursive = FALSE),
    context = unlist(lapply(res, `[[`, "context"), recursive = FALSE)
  )
}

cpp_attribute_pattern <- function(is_attribute) {
  paste0(
    "^[[:blank:]]*",
    if (!is_attribute) "//[[:blank:]]*",
    "\\[\\[",
    "[[:space:]]*(.*?)[[:space:]]*",
    "\\]\\]",
    "[[:space:]]*"
  )
}

# Parse a single C++ function signature into a one-row data.frame with
# columns `name`, `return_type`, `args` (a list of data.frames with columns
# `type`, `name`, `default`).
parse_cpp_function <- function(context, is_attribute = FALSE) {
  if (length(context) == 0 || !nzchar(context[[1L]])) {
    return(
      list(
        name = character(),
        return_type = character(),
        args = empty_args()
      )
    )
  }

  pattern <- cpp_attribute_pattern(is_attribute)

  # Drop pure-decoration lines.
  context <- grep(paste0(pattern, "$"), context, value = TRUE, invert = TRUE)

  if (is_attribute) {
    # Strip leading attributes when they share a line with the signature.
    context <- sub(pattern, "", context)
  }

  brace_or_stmt <- grep("[{;]", context)
  if (!length(brace_or_stmt)) {
    return(
      list(
        name = NA_character_,
        return_type = NA_character_,
        args = empty_args()
      )
    )
  }
  first_brace_or_statement <- brace_or_stmt[[1L]]

  # Remove trailing // comments and join.
  context <- sub("//.*", "", context[seq(1L, first_brace_or_statement)])
  signature <- sub("[[:space:]]*[{].*$", "", paste(context, collapse = " "))

  parse_cpp_signature(signature)
}

parse_cpp_signature <- function(signature) {
  signature <- trimws(signature)

  begin_paren <- regexpr("\\(", signature)
  end_paren <- max(gregexpr("\\)", signature)[[1L]])
  if (begin_paren < 1L || end_paren < 1L) {
    return(
      list(
        name = NA_character_,
        return_type = NA_character_,
        args = empty_args()
      )
    )
  }

  preamble <- trimws(substr(signature, 1L, begin_paren - 1L))
  # Split off the trailing identifier (function name, possibly qualified).
  sep <- regmatches(preamble, regexpr("[ \t\n\r\f\v*&][^ \t\n\r\f\v*&]*$", preamble))
  if (length(sep) == 0L || !nzchar(sep)) {
    name <- preamble
    return_type <- ""
  } else {
    name_start <- nchar(preamble) - nchar(sep) + 2L
    name <- substr(preamble, name_start, nchar(preamble))
    return_type <- trimws(substr(preamble, 1L, name_start - 2L))
  }

  args_str <- substr(signature, begin_paren + 1L, end_paren - 1L)
  args <- parse_cpp_arguments(args_str)

  list(
    name = name,
    return_type = return_type,
    args = args
  )
}

parse_cpp_arguments <- function(args) {
  args <- trimws(args)
  if (!nzchar(args) || identical(args, "void")) {
    return(empty_args())
  }

  chars <- strsplit(args, "", fixed = TRUE)[[1L]]
  pieces <- character()
  buf <- character()
  tmpl <- 0L
  paren <- 0L
  in_quote <- FALSE
  prev <- ""
  for (ch in chars) {
    if (ch == '"' && prev != "\\") {
      in_quote <- !in_quote
    }
    if (ch == "," && tmpl == 0L && paren == 0L && !in_quote) {
      pieces <- c(pieces, paste(buf, collapse = ""))
      buf <- character()
    } else {
      buf <- c(buf, ch)
      if (ch == "<") {
        tmpl <- tmpl + 1L
      } else if (ch == ">") {
        tmpl <- tmpl - 1L
      } else if (ch == "(") {
        paren <- paren + 1L
      } else if (ch == ")") paren <- paren - 1L
    }
    prev <- ch
  }
  if (length(buf)) {
    pieces <- c(pieces, paste(buf, collapse = ""))
  }
  pieces <- pieces[nzchar(trimws(pieces)) & trimws(pieces) != "void"]

  n <- length(pieces)
  type <- character(n)
  name <- character(n)
  default <- rep(NA_character_, n)

  for (i in seq_len(n)) {
    arg <- pieces[[i]]
    eq <- regexpr("=", arg, fixed = TRUE)
    if (eq > 0L) {
      default[[i]] <- trimws(substring(arg, eq + 1L))
      arg <- substring(arg, 1L, eq - 1L)
    }
    arg <- trimws(arg)
    m <- regexpr("[ \t\n\r\f\v*&][^ \t\n\r\f\v*&]*$", arg)
    if (m < 1L) {
      stop(sprintf("Argument %d (%s) has no type", i, pieces[[i]]), call. = FALSE)
    }
    # Name is the trailing identifier; type is everything else.
    nm <- sub(".*[ \t\n\r\f\v*&]", "", arg)
    ty <- substr(arg, 1L, nchar(arg) - nchar(nm))
    ty <- sub("[ \t\n\r\f\v]+$", "", ty)
    if (!nzchar(ty)) {
      stop(sprintf("Argument %d (%s) has no type", i, pieces[[i]]), call. = FALSE)
    }
    name[[i]] <- nm
    type[[i]] <- ty
  }

  list(type = type, name = name, default = default)
}

# Pure-R port of decor's blank_comments: replace all C/C++ comment characters
# (except newlines) with spaces so subsequent line-based parsing is safe.
blank_comments <- function(text) {
  chars <- strsplit(text, "", fixed = TRUE)[[1L]]
  n <- length(chars)
  state <- 0L # 0 normal, 1 single-line, 2 multi-line, 3 squote, 4 dquote
  i <- 1L
  while (i <= n) {
    ch <- chars[[i]]
    nxt <- if (i < n) chars[[i + 1L]] else ""
    if (state == 0L) {
      if (ch == "/" && nxt == "/") {
        chars[[i]] <- " "
        chars[[i + 1L]] <- " "
        state <- 1L
        i <- i + 2L
        next
      } else if (ch == "/" && nxt == "*") {
        chars[[i]] <- " "
        chars[[i + 1L]] <- " "
        state <- 2L
        i <- i + 2L
        next
      } else if (ch == "'") {
        state <- 3L
      } else if (ch == '"') {
        state <- 4L
      }
    } else if (state == 1L) {
      if (ch == "\n") {
        state <- 0L
      } else {
        chars[[i]] <- " "
      }
    } else if (state == 2L) {
      if (ch == "*" && nxt == "/") {
        chars[[i]] <- " "
        chars[[i + 1L]] <- " "
        state <- 0L
        i <- i + 2L
        next
      } else if (ch != "\n" && ch != "\r") {
        chars[[i]] <- " "
      }
    } else if (state == 3L) {
      if (ch == "\\" && i < n) {
        i <- i + 2L
        next
      }
      if (ch == "'") state <- 0L
    } else if (state == 4L) {
      if (ch == "\\" && i < n) {
        i <- i + 2L
        next
      }
      if (ch == '"') state <- 0L
    }
    i <- i + 1L
  }
  paste(chars, collapse = "")
}

# Compute the active C++ namespace at each line. Empty string for the global
# namespace. Anonymous namespaces are skipped (they need no qualifier).
namespace_per_line <- function(lines) {
  result <- character(length(lines))
  if (length(lines) == 0L) {
    return(result)
  }

  full <- paste(lines, collapse = "\n")
  pattern <- "namespace[[:space:]]+([A-Za-z_][A-Za-z0-9_:]*)[[:space:]]*\\{|\\{|\\}"
  matches <- gregexpr(pattern, full)[[1L]]
  if (matches[[1L]] == -1L) {
    return(result)
  }
  match_lens <- attr(matches, "match.length")

  # 1-based line of each character position.
  line_starts <- c(1L, cumsum(nchar(lines) + 1L) + 1L)
  line_starts <- line_starts[seq_len(length(lines))]
  match_lines <- findInterval(matches, line_starts)

  ns_stack_depths <- integer()
  ns_stack_names <- character()
  depth <- 0L
  cur_line <- 1L
  ns_str <- ""

  for (k in seq_along(matches)) {
    tok <- substr(full, matches[[k]], matches[[k]] + match_lens[[k]] - 1L)
    if (startsWith(tok, "namespace")) {
      # Fill up to and including the namespace line with the OLD ns_str so that
      # a decoration on the same line as `namespace foo {` is treated as being
      # in the outer scope.
      while (cur_line <= match_lines[[k]]) {
        result[[cur_line]] <- ns_str
        cur_line <- cur_line + 1L
      }
      nm <- sub("^namespace[[:space:]]+", "", tok)
      nm <- sub("[[:space:]]*\\{$", "", nm)
      depth <- depth + 1L
      ns_stack_depths <- c(ns_stack_depths, depth)
      ns_stack_names <- c(ns_stack_names, nm)
    } else {
      while (cur_line < match_lines[[k]]) {
        result[[cur_line]] <- ns_str
        cur_line <- cur_line + 1L
      }
      if (tok == "{") {
        depth <- depth + 1L
      } else if (tok == "}") {
        if (length(ns_stack_depths) && ns_stack_depths[[length(ns_stack_depths)]] == depth) {
          ns_stack_depths <- ns_stack_depths[-length(ns_stack_depths)]
          ns_stack_names <- ns_stack_names[-length(ns_stack_names)]
        }
        depth <- depth - 1L
      }
    }
    ns_str <- paste(ns_stack_names, collapse = "::")
  }
  while (cur_line <= length(lines)) {
    result[[cur_line]] <- ns_str
    cur_line <- cur_line + 1L
  }
  result
}
