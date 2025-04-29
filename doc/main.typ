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

Ha $vc(x)$ a felület egy pontja, $vc(n)$ pedig a felület normálvektora ebben a pontban,
az ennek megfelelő pont a duális térben $(vc(n), h)$, ahol:
$
  vc(x) dot vc(n) = h
$

Ha feltételezzük, hogy $vc(n)$ egység hosszúságú, akkor $h$ nem más,
mint az érintősík távolsága az origótól.
A $h(vc(s))$ függvényt a felület support függvényének hívjuk.

Ezzel a képlettel már át tudjuk transzformálni az interpolálandó adatpontokat a duális térbe.
Ahhoz, hogy a végeredményt leírhassuk a "primális" térben, szükségünk lesz az inverzre is,
tehát $vc(n)$-ből és $h$-ból ki szeretnénk számolni $vc(x)$-et.
Ehhez először fel kell írnunk néhány azonosságot.

$vc(x)(vc(s))$ parciális deriváltjai párhuzamosak az érintősíkkal
$
  dd(vc(x), vc(s))^T vc(n) = vc(0)
$

Így $h(vc(s))$ deriváltja
$
  dd(h, vc(s)) = dd(,vc(s)) vc(x)^T vc(n) = vc(x)^T dd(vc(n), vc(s))
$

Mivel $vc(n)(vc(s))$ egységhossúságú, egy gömbfelületet ír le.
Parciális deriváltjai merőlegesek rá
$
  dd(,vc(s)) vc(n) dot vc(n) = 2 med vc(n)^T dd(vc(n), vc(s)) = dd(, vc(s)) 1 = vc(0) \
  => dd(vc(n), vc(s))^T vc(n) = vc(0)
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
Az interpoláció közben szeretnénk biztosítani, hogy a hengeren maradunk.
Ennek érdekében bevezetünk egy új reprezentációt, az izotróp térben.
Ezt a reprezentációt úgy állítjuk elő,
hogy a $vc(w) = (0, 0, 1, 0)$ pontból az $n_z = 0$ hipersíkba vetítünk
$
  vc(y)(vc(b)) = 1 / (1 - n_z) vec(n_x, n_y, h)
$

Ennek az inverze
$
  vc(b)(vc(y)) = 1 / (1 + y_x^2 + y_y^2) vec(2 y_x, 2 y_y, -1 + y_x^2 + y_y^2, 2 y_z)
$

Az izotróp térben szabadon interpolálhatunk a transzformált adatpontok között,
majd a felületet visszavetítjük a Blaschke hengerre.

Bárhogy is interpoláljuk az adatpontjainkat az izotróp térben,
a visszatranszformált felület érintősíkjai meg fognak egyezni az előírtakkal.
Ahhoz viszont, hogy a konkrét térbeli pozíció is megegyezzen,
korlátoznunk kell a felület lehetséges deriváltjait az interpolációs pontokban
$
  vc(x)^T dd(vc(n), vc(s)) = dd(h, vc(s)) \
  vc(x)^T dd(vc(n), vc(y)) dd(vc(y), vc(s)) = dd(h, vc(y)) dd(vc(y), vc(s)) \
  underbrace((vc(x)^T dd(vc(n), vc(y)) - dd(h, vc(y))), vc(v)) dd(vc(y), vc(s)) = vc(0)
$

Ahol
$
vec(dd(vc(n), vc(y)), dd(h, vc(y))) = dd(vc(b), vc(y))= 2 / (1 + y_x^2 + y_y^2)^2
                       mat(1 - y_x^2 + y_y^2,         -2y_x y_y, 0;
                                   -2y_x y_y, 1 + y_x^2 - y_y^2, 0;
                                        2y_x,              2y_y, 0;
                                   -2y_x y_z,         -2y_y y_z, 1)
$

Tehát az izotróp térben kiválasztott kezdeti/végponti irányvektoroknak
illeszkedniük kell a $vc(v)$ normálvektorú, origót tartalmazó síkra.


= Coons-patch
Ha az adatpontjainkat áttranszformáltuk az izotróp térbe,
és rendeltünk hozzájuk megfelelő irányvektorokat (erre a következő pontban adunk egy heurisztikát),
akkor végre konstruálhatunk egy felületet.

= Folyamat

#chapter[Implementációs részletek]
= Polinom osztály
= Megjelenítés

#chapter[Eredmények]
