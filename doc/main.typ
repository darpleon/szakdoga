#import "templ.typ": template, todo, todo_image, chapter

#show: template.with(
  title: [Parametrikus görbék és felületek pontos offsetelése],
  subtitle: [Szakdolgozat],
  author: [Sandle Nátán],
  consulent: [Salvi Péter],
  digital: true
)

//#chapter(numbering: none)[Kivonat]

#chapter[Bevezetés]
= CAD/CAM
= Racionális görbék/felületek
= Kontrollpont-alapú reprezentáció
= Parametrikus sebesség

#chapter[PH Görbék]
= PH síkgörbék
== Alapok
== Reprezentáció komplex számokkal
== Interpoláció

= PH térgörbék
== Alapok
== Reprezentáció kvaterniókkal
== Interpoláció

#chapter[PN felületek]

#chapter[PN interpoláció $C^1$ folytonossággal]
= Duális reprezentáció
= Izotropikus tér
= Coons-patch
= Folyamat

#chapter[Implementációs részletek]
= Polinom osztály
= Megjelenítés

#chapter[Eredmények]
