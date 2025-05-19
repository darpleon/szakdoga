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
a

= Parametrikus görbék, felületek
Compu

= Polinomok, racionális függvények
Amikor geometriai alakzatokat szeretnénk szoftveresen reprezentálni,
figyelembe kell vennünk a számítógépek technikai limitációit.
A reprezentációban megjelenő matematikai kifejezéseket sokszor ki kell értékelnünk,
ennek az időigénye és pontossága pedig drasztikus mértékben függ a kifejezés jellegétől.

Az összeadást, kivonást és szorzást nagyon egyszerű algoritmusokkal,
akár 1 CPU-ciklus alatt végre tudjuk hajtani,
// TODO: adatszerkezet?
az eredmény pontossága csak a számok mögötti adatszerkezet (általában floating-point) limitációitól függ.

Azokat a függvényeket, amik kifejezhetők véges sok összeadással, kivonással és szorzással,
polinomoknak hívjuk. Egy egyváltozós polinom kanonikus alakja 
$
  P(x) = sum_(k = 0)^n a_k x^k
$

Az osztás egy kissé költségesebb (illetve adott esetben pontatlanabb) művelet.
Ha az osztást is megengedjük, az így kifejezhető függvényeket racionális függvényeknek hívjuk.
Minden racionális függvény leírható az alábbi alakban
$
  R(x) = A(x) / B(x)
$
ahol $A(x)$ és $B(x)$ polinomok.
Ez kedvező, mert így egy racionális függvény kiértékelésekor elég csak egyszer osztani.

Sok nevezetes függvényt (például $sqrt(x)$, $sin(x)$, $ln(x)$)
nem lehet kifejezni véges sok alapművelettel,
értéküket csak megközelíteni tudjuk.
Ezt vagy egy közelítő polinommal/racionálissal tesszük (pl Taylor-sor, Padé közelítő),
vagy ismételt, inkrementálisan közelítő lépéseket hajtunk végre (pl Newton-módszer).

Ebből következik, hogy az ilyen függvények kiértékelése lassabb, pontatlanabb, vagy mindkettő,
mint egy alacsony fokú polinom vagy racionális függvény.
Így lehetőség szerint el akarjuk őket kerülni egy CAD környezetben.

= Kontrollpont-alapú reprezentáció
Ha egy görbét/felületet meghatározó polinomot a szokásos hatványösszeg alakban írunk le,
az együtthatók nem nyújtanak intuitív betekintést a görbe/felület geometriai tulajdonságaiba.
A CAD-ben elterjedtek olyan alternatív reprezentációk,
melyek.

A kontrollpontok tekinthetők együtthatóknak egy másik bázisban,
de léteznek


== Bézier görbék
Egy $n$-ed fokú Bézier görbét $n + 1$ kontrollponttal reprezentálunk.
Kiértékelni a De Casteljau algoritmussal tudjuk,
ami rekurzív lineáris interpolációra épül.
A Béziér kontrollpontok a görbe mögötti polinom együtthatói a Bernstein-bázisban,
melynek $k$-adik eleme
$
  b_(k, n)(t) = binom(n, k) t^k (1 - t)^(n - k)
$
A Béziér görbe $t = 0$-ban áthalad az első kontrollponton,
$t = 1$-ben az utolsón, a többit pedig közelíti.
Az első illetve utolsó kettő kontrollpontot összekötő egyenes
érinti a görbét az első illetve utolsó kontrollpontban.
Kifejezetten népszerű a harmadfokú Bézier görbe a graphic design területén,
hiszen egyszerűen lehet állítani a görbe irányait a végpontokban.
#todo_image("Bézier kép")

== B-Spline
A B-Spline (Basis-Spline) darabonként definiált bázisfüggvényekből áll,
melyeknek szegmenseit úgynevezett "csomópontok" (knots) választják el ($t_0, t_1 dots t_m$).
A bázisfüggvényeket A Cox-de Boor képlettel tudjuk kiértékelni:
$
  B_(i, 0)(t) := cases(1 wide "ha" t_i <= t < t_(i + 1), 0 wide "egyébként") \
  B_(i, n)(t) := (t - t_i) / (t_(i + n) - t_i) + (t_(i + n + 1) - t) / (t_(i + n + 1) - t_(i + 1))
$

Ebből következik, hogy egy kontrollpont csak a környező $n + 1$ szegmensre hat ki,
így lehetőséget ad a lokális kontrollra.

A B-Spline egyik fő előnye, hogy "maximális folytonosságot" biztosít a szegmensek között,
$n$-edfokú spline esetén $C^(n - 1)$-et.
Azonban általános esetben az egyik kontrollponton sem megy át, csak közelíti őket.
Csomópontok ismétlésével elérhető, hogy a görbe átmenjen egy kontrollponton,
ez azonban a folytonosság vesztésével jár.
Mivel ez nem okoz gondot az első és utolsó kontrollpontban,
ott gyakran megteszik (clamping).
#todo_image("B-Spline kép")


== NURBS
A NURBS valójában nem más, mint a B-Spline általános esete.
A rövidítés kifejtése: "Non-Uniform Rational B-Spline".

A "non-uniform" rész azt jelenti, hogy nem feltétlenül vannak a csomópontok egyenlő távolságra egymástól,
így például megengedett a korábban említett csomópont ismétlés is.
A kontrollpontok sűrítése a görbe/felület adott szakaszain lehetőséget ad
a finomabb részletek lokális szerkesztésére.
Az úgynevezett "knot insertion" algoritmussal hozzá tudunk adni egy új csomópontot egy B-Spline-hoz,
anélkül, hogy annak az alakját változtatnánk.

A "rational" rész azt jelenti, hogy egyszerű polinomok helyett racionális függvények vannak a háttérben.
Ez a gyakorlatban úgy nyilvánul meg, hogy minden kontrollponthoz rendelünk egy súlyt.
Számoláskor az adott kontrollponthoz tartozó komponenst beszorozzuk a súllyal,
majd végül osztunk a bázisfüggvények súlyozott összegével.
Nagyobb súly hatására a görbe nagyobb mértékben fog húzni az adott kontrollpont irányába.

A súlyokat értelmezhetjük a számítógépes grafikában elterjedt projektív geometriával.
Elképzelhetjük, hogy a spline egy egyel nagyobb dimenziós térben él,
ahol az utolsó koordináta a súly (homogén koordináták).
Így az osztás nem más, mint vetítés az eredeti térbe.

A racionális függvényekre való kiterjeszkedés lehetővé teszi a körívek/gömbfelületek pontos leírását.

#todo_image("NURBS kép")

= Parametrikus sebesség
Egy $vc(r)(t)$ görbe parametrikus sebessége alatt a görbe deriváltjának nagyságát értjük.
Ezt a koordinátánként vett deriváltakból a pitagoraszi távolságképlettel tudjuk kiszámolni,
ami egy síkgörbe esetén így néz ki:
$
  sigma(t) = |vc(r)'(t)| = sqrt(x'(t)^2 + y'(t)^2)
$

A parametrikus sebesség ismerete fontos lehet CAD/CAM környezetben,
márcsak azért is, mert segítségével további hasznos dolgokat ki tudunk számolni.
A korábbi definícióból következik, hogy a parametrikus sebesség integrálásával megkapjuk a görbe hosszát.
Az előjeles görbületet ki tudjuk számolni az alábbi képlettel
$
  k = (x'y'' - y'x'') / sigma^3
$

Számunkra a legfontosabb az adott távolságra lévő "párhuzamos görbe" (más szóval offset) képlete lesz
$
  vc(r)_d = vc(r) + d vc(n)
$
ahol $vc(n)$ az $vc(r)$ görbére állított egységhosszúságú normálvektor.
Normálvektort egyszerűen kapunk úgy, hogy merőlegest állítunk a deriváltra.
De ahhoz, hogy ez egységhosszúságú legyen, le kell osztanunk a derivált nagyságával,
tehát a parametrikus sebességgel. Így a képlet
$
  vc(r)_d = vc(r) + d vc(r)'_perp / sigma
$

Sajnos a parametrikus sebességgel van egy jelentős probléma a gyakorlati felhasználás terén:
a négyzetgyök miatt egy polinomiális/racionális görbe parametrikus sebessége általános esetben nem polinomiális/racionális.
Ez nem csak azért okoz gondot, mert költségesebbé teszi a kiértékelést, hanem azért is,
mert így nem tudjuk az offsetet kifejezni a szokásos kontrollpont alapú módszerekkel.
Így CAD/CAM rendszerekben gyakran pontatlan közelítéseket kell alkalmazni.

#chapter[Polinomiális PH Görbék]
A "Pythagorean-hodograph" (PH) görbék olyan speciális polinomiális/racionális görbék,
melyeknek a parametrikus sebessége is polinomiális/racionális.
A névben a "hodográf" kifejezés valamilyen mozgás sebességének (tehát deriváltjának) a grafikonját jelenti.
Ilyen ábrákat használnak például szélsebességek elemzésére.
A "pitagoraszi" alatt pedig a Pitagorasz tételt értjük egy derékszögű háromszög oldalai közti összefüggésre.
Összesítve, PH görbe alatt olyan parametrikus görbéket értünk, ahol az alábbi kifejezésben
$
  x'(t)^2 + y'(t)^2 = sigma(t)^2
$
minden tag, tehát a parametrikus sebesség is, egy polinom, vagy racionális függvény
(még a négyzetre emelés előtt is).
= PH síkgörbék
Ugyanúgy, ahogy léteznek $a$, $b$, $c$ pitagoraszi (egész) számhármasok,
melyekre teljesül, hogy $a^2 + b^2 = c^2$,
léteznek $a(t)$, $b(t)$, $c(t)$ pitagoraszi #emph[polinom]hármasok is.
Ilyen polinomhármasokat tudunk generálni is,
méghozzá ugyanazzal a képlettel, amivel a számhármasokat.

Legyen $m(t)$ és $n(t)$ két polinom.
Ekkor a polinomhármasunk
$
  a(t) &= m^2(t) - n^2(t) \
  b(t) &= 2 m(t) n(t) \
  c(t) &= m^2(t) + n^2(t)
$

A mi parametrikus görbénkben
$
  x'(t) = a(t) wide wide y'(t) = b(t) wide wide sigma(t) = c(t)
$
A görbét magát megkaphatjuk a deriváltak integrálásával (és kiindulópont választásával).

== Reprezentáció komplex számokkal
A PH görbék generálásának fent leírt módját elegánsabban ki tudjuk fejezni, ha komplex számokat használunk.
Legyen
$
  vc(w)(t) = m(t) + i n(t)
$
Ekkor
$
  vc(w)^2(t) &= m(t)^2 - n(t)^2 + 2 i m(t) n(t) \
  &= x'(t) + i y'(t)
$
Továbbá
$
  |vc(w)(t)|^2 &= vc(w)(t) vc(w)^*(t) = m^2(t) + n^2(t) \ &= sigma(t)
$
Végül
$
  vc(r)(t) = x(t) + i y(t)
$

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
// TODO: h(s) vagy h(n)?

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

Tehát az izotróp térben kiválasztott kezdeti/végponti deriváltaknak
illeszkedniük kell a $vc(v)$ normálvektorú, origót tartalmazó síkra.

= Irányvektorok meghatározása
Jelenleg rendelkezünk egy négyzetrács szerkezetű ponthálózattal,
illetve pontonként egy síkkal.
Mivel ezekből még nem következnek egyértelműen a pontokhoz rendelendő deriváltak,
heurisztikát fogunk alkalmazni.

Legyen $vc(a)_(i, j)$ a hálózat egy pontja, ahol $i$ a pont "$u$ irányban", $j$ pedig a "$v$ irányban" vett indexe.
Legyen továbbá $n$ és $m$ a legmagasabb $i$, illetve $j$ index.
Jelölje $vc(gamma)_(i, j)$ a felület $u$ szerinti deriváltját az $vc(a)_(i, j)$ pontban,
$vc(delta)_(i, j)$ pedig a $v$ szerinti deriváltat ugyanitt.

// TODO: egy kis szóbeli magyarázat?
Ha $vc(a)_(i, j)$ a pontháló szélén van
#centered_split(
  $
    vc(gamma)^*_(0, j) =& vc(a)_(1, j) - vc(a)_(0, j) \
    vc(gamma)^*_(n, j) =& vc(a)_(n, j) - vc(a)_(n - 1, j) \
  $,
  $
    vc(delta)^*_(i, 0) =& vc(a)_(i, 1) - vc(a)_(i, 0) \
    vc(delta)^*_(i, n) =& vc(a)_(i, n) - vc(a)_(i, n - 1) \
  $
)

Egyébként a két vektort átlagoljuk
#centered_split(
  $
    vc(gamma)^*_(i, j) =& (vc(a)_(i + 1, j) - vc(a)_(i - 1, j)) / 2 \
  $,
  $
    vc(delta)^*_(i, j) =& (vc(a)_(i, j + 1) - vc(a)_(i, j - 1)) / 2 \
  $
)

A kapott vektorokat még le kell vetítenünk a $vc(v)$ által meghatározott síkra
#centered_split(
  $
    vc(gamma)_(i, j) = vc(gamma)^*_(i, j) - (vc(v) dot vc(gamma)^*_(i, j)) / (vc(v) dot vc(v)) vc(v)
  $,
  $
    vc(delta)_(i, j) = vc(delta)^*_(i, j) - (vc(v) dot vc(delta)^*_(i, j)) / (vc(v) dot vc(v)) vc(v)
  $
)

= Coons-patch
Vegyünk a ponthálónkból egy kis négyzetet,
ennek sarokpontjait nevezzük $vc(a)_00$, $vc(a)_01$, $vc(a)_10$, $vc(a)_11$-nek.
Ezek között akarunk interpolálni úgy, hogy a létrejött felületdarab
a vele szomszédos felületdarabokra $C^1$ folytonossággal illeszkedjen.
Ehhez egy Coons patch-et fogunk használni.

A Coons patch létrehozásához szükségünk van 4 határgörbére \
($vc(c)_0 (u)$, $vc(c)_1 (u)$, $vc(d)_0 (v)$, $vc(d)_0 (v)$), ahol
$
  vc(c)_0 (0) = vc(d)_0 (0) = vc(a)_00 \
  vc(c)_0 (1) = vc(d)_1 (0) = vc(a)_10 \
  vc(c)_1 (0) = vc(d)_0 (1) = vc(a)_01 \
  vc(c)_1 (1) = vc(d)_1 (1) = vc(a)_11 \
$

valamint egy $0$ és $1$ között interpoláló $F(t)$ függvényre.
Az egyszerűség kedvéért a függvény tükörképét is nevezzük meg
$
  F_0(t) &= 1 - F(t) \
  F_1(t) &= F(t)
$

A Coons patch három részből áll.
Az első kettő interpolál az egymással szemben álló görbék között
$
  vc(S)_c (u, v) &= F_0(v) vc(c)_0(u) + F_1(v) vc(c)_1(u) \
  vc(S)_d (u, v) &= F_0(u) vc(d)_0(v) + F_1(u) vc(d)_1(v)
$

// TODO: ez általános esetben is ilyen egyszerű?
A harmadik pedig interpolál a sarokpontok között
$
  vc(B) (u, v) &= F_0(u)F_0(v) vc(a)_00 +
                  F_0(u)F_1(v) vc(a)_01 \
           &+ med F_1(u)F_0(v) vc(a)_10 +
                  F_1(u)F_1(v) vc(a)_11
$

Végül
$
  vc(y)(u, v) = vc(S)_c (u, v) + vc(S)_d (u, v) - vc(B)(u, v)
$

A képletet értelmezhetjük úgy,
hogy $vc(S)_c$ és $vc(S)_d$ összeadásával "kétszer interpoláltunk" a sarokpontok között,
ezt kompenzáljuk $vc(B)$ kivonásával.

A Coons patch kifejezhető egy kompaktabb mátrix alakban is
$
  vc(y)(u, v) = mat(F_0(u), 1, F_1(u)) dot mat(-vc(a)_00,     vc(d)_00 (v), -vc(a)_01;
                                             vc(c)_00 (u), 0,          vc(c)_01 (u);
                                            -vc(a)_10,     vc(d)_10 (v), -vc(a)_11) dot vec(F_0(v), 1, F_1(v))
$

Az interpoláló függvény általában lineáris vagy köbös szokott lenni.
Ahhoz, hogy a patch-ek $C^1$ folytonossággal illeszkedjenek, nekünk köbösre lesz szükségünk
$
  F_0(t) = 2t^3 - 3t^2 + 1 \
  F_0(t) = -2t^3 + 3t^2
$

A határgörbékhez használjunk Hermite interpolációt
#centered_split(
$
  vc(c)_0(u) = vec(F_0(u), G_0(u), F_1(u), G_1(u)) dot
               vec(vc(a)_00, vc(gamma)_00, vc(a)_10, vc(gamma)_10) \
  vc(c)_0(u) = vec(F_0(u), G_0(u), F_1(u), G_1(u)) dot
               vec(vc(a)_00, vc(gamma)_00, vc(a)_10, vc(gamma)_10) \
$,
$
  vc(d)_0(v) = vec(F_0(v), G_0(v), F_1(v), G_1(v)) dot
               vec(vc(a)_00, vc(gamma)_00, vc(a)_10, vc(gamma)_10) \
  vc(d)_0(v) = vec(F_0(v), G_0(v), F_1(v), G_1(v)) dot
               vec(vc(a)_00, vc(gamma)_00, vc(a)_10, vc(gamma)_10) \
$,
)

ahol
$
  G_0(t) &= t^3 - 2t^2 + t \
  G_1(t) &= -2t^3 + 3t^2
$

Mivel $F_0$ és $F_1$ ugyanaz a Coons patch képletében, mint a határgörbékében,
a kettőt összevonva megspórolhatjuk a görbék külön kiszámolását
$
  vc(y)(u, v) = mat(F_0(u), G_0(u), F_1(u), G_1(u))
  mat(vc(a)_00, vc(delta)_00, vc(a)_01, vc(delta)_01;
      vc(gamma)_00, vc(0), vc(gamma)_01, vc(0);
      vc(a)_10, vc(delta)_10, vc(a)_11, vc(delta)_11;
      vc(gamma)_10, vc(0), vc(gamma)_11, vc(0);)
  vec(F_0(v), G_0(v), F_1(v), G_1(v))
$

Mivel nem szorzunk össze azonos változótól függő függvényeket,
a deriváltak is hasonlóan néznek ki
$
  dd(vc(y), u) = mat(F'_0(u), G'_0(u), F'_1(u), G'_1(u))
  mat(vc(a)_00, vc(delta)_00, vc(a)_01, vc(delta)_01;
      vc(gamma)_00, vc(0), vc(gamma)_01, vc(0);
      vc(a)_10, vc(delta)_10, vc(a)_11, vc(delta)_11;
      vc(gamma)_10, vc(0), vc(gamma)_11, vc(0);)
  vec(F_0(v), G_0(v), F_1(v), G_1(v)) \

  dd(vc(y), v) = mat(F_0(u), G_0(u), F_1(u), G_1(u))
  mat(vc(a)_00, vc(delta)_00, vc(a)_01, vc(delta)_01;
      vc(gamma)_00, vc(0), vc(gamma)_01, vc(0);
      vc(a)_10, vc(delta)_10, vc(a)_11, vc(delta)_11;
      vc(gamma)_10, vc(0), vc(gamma)_11, vc(0);)
  vec(F'_0(v), G'_0(v), F'_1(v), G'_1(v))
$

= Visszatranszformálás

#chapter[Implementációs részletek]
= overview
C++
= Lineáris algebra
= Polinom osztály
A polinomok szimbolikus reprezentációjához és manipulációjához létrehoztam a `Polynomial` osztályt.
== Template
Az osztályban használom a C++ template funkcióját.
- Maga az osztály templatelt az együtthatók típusára ( `coeff_type` )
- A polinomot kiértékelő `evaluate` függvény templatelt a bemeneti paraméter típusára ( `input_type` )

Az `evaluate` függvény visszatérési értékét az alábbi template kifejezés írja le
```cpp
  using result_type = sum_type<product_type<
                                   product_type<input_type>,
                                   coeff_type>>;
```
A `sum_type` és `product_type` templatek két típus (vagy egy típus önmagával vett)
összegét, illetve szorzatát ( `operator+` / `operator*` ) fejezik ki, és kódban így néznek ki
```cpp
template <typename T, typename U = T>
using sum_type = decltype(std::declval<T>() + std::declval<U>());

template <typename T, typename U = T>
using product_type = decltype(std::declval<T>() * std::declval<U>());
```
Létrehoztam továbbá egy templatelt structot (neve `I`, az "Identity" szót rövidítve)
aminek két statikus tagja, `zero` és `one` tárolják azokat az értékeket,
amik az adott típus nullája, illetve egységeként értendőek.
Ezt a structot specializálni kell a használt típusokra ahhoz,
hogy bizonyos függvényeket (pl. `evaluate` ) használni lehessen.

A templatek használata lehetővé teszi a felhasználást különböző típusokkal
(pl float, double, complex, vektor).
Nagyban növeli a rugalmasságot, hogy az együtthatók és a bemeneti paraméter külön vannak templatelve.
Így például lehet polinomunk ami double típusú paramétert kap,
de az együtthatói (így végül a `result_type`-ja is) vektor típusúak.

És mivel az `input_type`-ot csak az `evaluate` meghívásakor kell meghatározni,
ugyanazt a `Polynomial` példányt különböző típusú paraméterekkel ki lehet értékelni.

Egy kifejezetten érdekes és hasznos következménye a template-es működésnek az,
hogy két polinomot képesek vagyunk komponálni úgy, hogy az egyiket kiértékeljük a másikban.
== Variable

== PowerPermutation

== Rational
= Megjelenítés

#chapter[Eredmények]
