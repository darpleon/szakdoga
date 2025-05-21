#let todo = text.with(red)
#let todo_image(content, width: 7.5cm, height: 7.5cm) = {
  align(center)[
    #rect(width: width, height: height, stroke: red)[
      #v(1fr)
      #todo(content)
      #v(1fr)
    ]
  ]
}

#let chapter = heading.with(level: 1)

#let titlepage(
  title: [Cím],
  subtitle: [Alcím],
  author: [Mézga Géza],
  consulent: [Gézga Méza],
  date: [#datetime.today().display()],
) = {
  page(numbering: none)[
    #align(center)[
      #image("bme_logo_nagy.svg", width: 40%)
      *Budapesti Műszaki és Gazdaságtudományi Egyetem*\
      Villamosmérnöki és Informatikai Kar\
      Irányítástechnika és Informatika Tanszék
      #v(1fr)
      #text(size: 2em, weight: "bold", title)
      #v(1em)
      #text(size: 1.5em)[#smallcaps(subtitle)]
      #v(0.7fr)
      #grid(columns: (1fr, 1fr),
        [
          _Készítette_\
          #author
        ],
        [
          _Konzulens_\
          #consulent
        ]
      )
      #v(1.3fr)
      #date
    ]
  ]
}

#let student_statement(
  student: [Mézga Géza],
  date: [#datetime.today().display()],
) = {
  page(numbering: none)[
    #let stud = emph(student)
    #align(center)[
      #text(size: 1.1em)[
        *HALLGATÓI NYILATKOZAT*
      ]
    ]
    #v(1em)

    Alulírott #stud, szigorló hallgató kijelentem, hogy ezt a szakdolgozatot meg
    nem engedett segítség nélkül, saját magam készítettem, csak a megadott forrásokat
    (szakirodalom, eszközök stb.) használtam fel. Minden olyan részt, melyet szó szerint,
    vagy azonos értelemben, de átfogalmazva más forrásból átvettem, egyértelműen, a forrás
    megadásával megjelöltem.

    Hozzájárulok, hogy a jelen munkám alapadatait (szerző(k), cím, angol és magyar nyelvű
    tartalmi kivonat, készítés éve, konzulens(ek) neve) a BME VIK nyilvánosan hozzáférhető
    elektronikus formában, a munka teljes szövegét pedig az egyetem belső hálózatán keresztül
    (vagy autentikált felhasználók számára) közzétegye. Kijelentem, hogy a benyújtott munka
    és annak elektronikus verziója megegyezik. Dékáni engedéllyel titkosított diplomatervek
    esetén a dolgozat szövege csak 3 év eltelte után válik hozzáférhetővé.
    #v(3em)
    Budapest, #date
    #v(3em)
    #align(end)[
      #box[
        #align(center)[
          #line(length: 6cm)
          #stud\
          hallgató
        ]
      ]
      #h(1em)
    ]
  ]
}

#let format(digital: false, body) = {
  set text(font: "New Computer Modern", size: 12pt, lang: "hu")
  set heading(numbering: "1.")
  let margin = if digital {
    (left: 3.5cm, rest: 2.5cm)
  } else {
    (inside: 3.5cm, rest: 2.5cm)
  }
  set page(
    paper: "a4",
    margin: margin,
    numbering: "1",
  )
  set outline(indent: auto)
  set ref(supplement: none)

  set outline.entry(fill: repeat[#" . "])

  show outline.entry.where(level: 1): set outline.entry(fill: [ ])
  show outline.entry.where(level: 1): set block(above: 1.5em)
  show outline.entry.where(level: 1): strong

  show heading.where(level: 1): it => {
    pagebreak(weak: true)
    v(2cm)
    if it.numbering != none {
      text(size: 1.25em)[
        #counter(heading).display(it.numbering)
        #it.supplement
      ]
      v(0.5em)
    }
    text(size: 1.5em)[#it.body]
    v(1.5em)
  }

  show heading: it => {
    it
    v(0.5em)
  }
  set heading(offset: 1)
  set par(justify: true)

  show raw: it => {
    let params = (fill: rgb("eeeeee"), radius: 0.3em, outset: 0.3em)
    if it.block {
      block(it, width: 100%, ..params)
    } else {
      box(it, ..params)
    }
  }

  body
}

#let template(
  title: [Cím],
  subtitle: [Alcím],
  author: [Mézga Géza],
  consulent: [Gézga Méza],
  date: datetime.today().display(),
  showtitle: true,
  showoutline: true,
  showstatement: true,
  digital: true,
  body
) = {
  show: format.with(digital: digital)

  if showtitle {
    titlepage(
      title: title,
      subtitle: subtitle,
      author: author,
      consulent: consulent,
      date: date
    )
  }

  if showoutline {
    set page(numbering: none)
    set heading(offset: 0)
    outline()
  }
  if showstatement {
    student_statement(student: author, date: date)
  }
  counter(page).update(1)

  body
}
