#let vc(symbol) = math.upright(math.bold(symbol))
#let vr(symbol) = math.bold(symbol)

#let dp(x, y) = $(diff #x) / (diff #y)$
#let dd(x, y) = $(d #x) / (d #y)$

#let centered_split(distance: 40%, ..cells) = {
  align(center)[#block(width: 2 * distance)[#grid(columns: (1fr, 1fr), row-gutter: 10pt, ..cells)]]
}
