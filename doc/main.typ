#import "templ.typ": template, todo, todo_image, chapter
#import "utils.typ": *

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
= Feladat
= Duális reprezentáció
Egy olyan $vc(x)(vc(s))$ racionális felületet keresünk,
melynek egységhosszúságú normálvektorait leíró $vc(n)(vc(s))$ függvény szintén racionális.
Kézenfekfő lehet "fordítva gondolkozni": először konstruálni egy garantáltan racionális $vc(n)(vc(s))$-t,
majd ebből meghatározni $vc(x)(vc(s))$-t.
Felületünket a szokásos $(x, y, z)$ koordináták helyett reprezentálhatjuk az úgynevezett "duális térben",
$(n_x, n_y, n_z, h)$ koordinátákkal.
Ezek a koordináták a felület egy pontja helyett a felület egy érintősíkját írják le.

Legyen $vc(p)$ a felület egy pontja, $vc(n)$ pedig a felület normálvektora ebben a pontban.
Az ennek megfelelő pont a duális térben $(vc(n), h)$, ahol:
$
  vc(p) dot vc(n) = h
$

Ha feltételezzük, hogy $vc(n)$ egység hosszúságú, akkor $h$ nem más,
mint az érintősík távolsága az origótól.
A $h(vc(s))$ függvényt a felület support függvényének hívjuk.

A mi módszerünkben $vc(n)(vc(s))$-t és $h(vc(s))$-t fogjuk ismerni,
ezekből kell kiszámolnunk $vc(x)(vc(s))$-t.
Ehhez először fel kell írnunk néhány azonosságot.

$vc(x)(vc(s))$ parciális deriváltjai párhuzamosak az érintősíkkal
$
  dd(vc(x), vc(s)) vc(n) = vc(0)
$

Így $h(vc(s))$ deriváltja
$
  dd(h, vc(s)) = dd(,vc(s)) vc(x)^T dot vc(n) = vc(x)^T dot dd(vc(n), vc(s))
$

Mivel $vc(n)(vc(s))$ egységhossúságú, egy gömbfelületet ír le.
Parciális deriváltjai merőlegesek rá
$
  dd(,vc(s)) vc(n) dot vc(n) = 2 dd(vc(n), vc(s)) dot vc(n) = dd(, vc(s)) 1 \
  => dd(vc(n), vc(s)) dot vc(n) = vc(0)
$

$h vc(n)$ egy pont az érintősíkon,
$dd(vc(n), u)$ és $dd(vc(n), v)$ pedig az érintősíkkal párhuzamos vektorok.
Így $vc(x)$-et ki tudjuk fejezni az alábbi módon
$
  vc(x) = h vc(n) + dd(vc(n), vc(s)) dot vc(r)
$

Szorozva $dd(vc(n), vc(s))^T$-al
$
  dd(h, vc(s))^T = dd(vc(n), vc(s))^T dd(vc(n), vc(s)) dot vc(r) \
  vc(r) = (dd(vc(n), vc(s))^T dd(vc(n), vc(s)))^(-1) dd(h, vc(s))^T
$

Tehát
$
  vc(x) = h vc(n) + dd(vc(n), vc(s)) (dd(vc(n), vc(s))^T dd(vc(n), vc(s)))^(-1) dd(h, vc(s))^T
$

= Izotróp tér
Az egységhosszúságú normálvektor előírásával $RR^4$-et leszűkítettük $cal(B)$-re,
az úgynevezett Blaschke hengerre.
Ahhoz, hogy


= Coons-patch
= Folyamat

#chapter[Implementációs részletek]
= Polinom osztály
= Megjelenítés

#chapter[Eredmények]
