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
A számítógépek elterjedésével megjelent az iparban a
"Computer-Aided Design" (CAD) és "Computer-Aided Machining" (CAM) fogalma.
Ezek grafikus szoftverfelületek, melyek lehetővé teszik ipari alkatrészek,
fogyasztói termékek, vagy akár épületek digitális tervezését,
illetve digitálisan irányított gyártását (Computer Numerical Control - CNC).
Ennek szolgálatára alakultak ki és fejlődnek tovább az alkalmazott matematika bizonyos ágai,
mint például a numerikus analízis.

A CAD/CAM rendszerek alapvető eszköze a polinomiális/racionális interpoláció,
illetve az erre épülő parametrikus görbék/felületek.
A dolgozat célja, hogy rávilágítson ezeknek egy speciális alosztályára:
az úgynevezett "pitagoraszi-hodográf" (PH) görbékre, és "pitagoraszi-normál" (PN) felületekre.
A PH görbék/PN felületek különleges tulajdonságaikból adódóan
egyszerűsítik az "offsetek" (párhuzamos görbék/felületek) előállítását.

A PH görbék kutatásának úttörője Rida T. Farouki,
aki 1990-ben vezette be a fogalmat @first,
és azóta is aktívan tevékenykedik a területen.
A téma 1995-ben Helmut Pottman cikkével @pottman kibővült a PN felületekkel,
a PH görbék felületi megfelelőjével.

A dolgozat első pár fejezetébem bemutatom a PH/PN mögötti motivációkat,
ismertetem a pitagoraszi görbék/felületek elméletének alapjait.
Ezután kitérek a PN felületek interpolációjának egy konkrét módszerére,
melyet Lávička et al. fogalmazott meg 2016-ban @smooth.
Kifejtem az egyes lépések mögötti matematikát.
Beszámolok a módszer szoftveres implementációjáról, illetve az elért eredményekről.

#pagebreak()

= Polinomok, racionális függvények
Amikor geometriai alakzatokat szeretnénk szoftveresen reprezentálni,
figyelembe kell vennünk a számítógépek technikai limitációit.
A reprezentációban megjelenő matematikai kifejezéseket sokszor ki kell értékelnünk,
ennek az időigénye és pontossága pedig drasztikus mértékben függ a kifejezés jellegétől.

Az összeadást, kivonást és szorzást nagyon egyszerű algoritmusokkal,
akár 1 CPU-ciklus alatt végre tudjuk hajtani,
az eredmény pontossága csak a számok mögötti adatszerkezet (általában floating-point) limitációitól függ.

Azokat a függvényeket, amik kifejezhetők véges sok összeadással, kivonással és szorzással,
polinomoknak hívjuk. Egy egyváltozós polinom kanonikus alakja 
(az úgynevezett "monomiális bázis"-ban)
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
Ezt vagy egy közelítő polinommal/racionálissal tesszük (pl. Taylor-sor, Padé közelítő),
vagy ismételt, inkrementálisan közelítő lépéseket hajtunk végre (pl. Newton-módszer).

Ebből következik, hogy az ilyen függvények kiértékelése lassabb, pontatlanabb, vagy mindkettő,
mint egy alacsony fokú polinom vagy racionális függvény.
Így lehetőség szerint el akarjuk őket kerülni egy CAD környezetben.

= Kontrollpont-alapú reprezentáció
Ha egy görbét/felületet meghatározó polinomot a szokásos hatványösszeg alakban írunk le,
az együtthatók nem nyújtanak intuitív betekintést a görbe/felület geometriai tulajdonságaiba.
Léteznek azonban olyan alternatív polinomiális bázisok, melyekben az együtthatók térbeli pozíciója
(értsd: a koordinátánként vett együtthatókat vektorként értelmezzük; ez a "kontrollpont")
valamilyen módon tükrözi a görbe elhelyezkedését és alakját.


== Bézier görbék
Egy $n$-ed fokú Bézier görbét $n + 1$ kontrollponttal reprezentálunk.
Kiértékelni a De Casteljau algoritmussal tudjuk,
ami rekurzív lineáris interpolációra épül.
A Bézier kontrollpontok a görbe mögötti polinom együtthatói a Bernstein-bázisban,
melynek $k$-adik eleme
$
  b_(k, n)(t) = binom(n, k) t^k (1 - t)^(n - k)
$
A Bézier görbe $t = 0$-ban áthalad az első kontrollponton,
$t = 1$-ben az utolsón, a többit pedig közelíti.
Az első illetve utolsó kettő kontrollpontot összekötő egyenes
érinti a görbét az első illetve utolsó kontrollpontban.
Kifejezetten népszerű a harmadfokú Bézier görbe a graphic design területén,
hiszen egyszerűen lehet állítani a görbe irányait a végpontokban.
#figure(
  image("images/bezier.png", width: 70%),
  caption: [egy harmadfokú Bézier görbe kontrollpontjaival (forrás: Wikipédia)]
)

== B-Spline
A B-Spline (Basis-Spline) darabonként definiált bázisfüggvényekből áll,
melyeknek szegmenseit úgynevezett "csomópontok" (knots) választják el ($t_0, t_1, dots, t_m$).
A bázisfüggvényeket a Cox-de Boor képlettel tudjuk kiértékelni:
$
  N_(i, 0)(t) := cases(1 wide "ha" t_i <= t < t_(i + 1), 0 wide "egyébként") \
  N_(i, n)(t) := (t - t_i) / (t_(i + n) - t_i) + (t_(i + n + 1) - t) / (t_(i + n + 1) - t_(i + 1))
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
#figure(
  image("images/bsbasis.png", width: 70%),
  caption: [negyedfokú B-Spline bázis (forrás: MIT)]
)


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

A racionális függvényekre való kiterjeszkedés lehetővé teszi a körívek/gömbfelületek pontos leírását is.

Széleskörű funkcionalitása miatt a NURBS spline-ok használata industry standard a CAD/CAM területén.

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

A következő fejezetben megismerkedünk a PH görbékkel,
amik megoldást nyújtanak erre a problémára.

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
Hermite adatok (végponti érték/derivált) interpolálásához az előző pont képletei alapján
fel lehet írni egy egyenletrendszert $vc(w)(t)$ együtthatóira.

Ahhoz, hogy mindkét végpontban interpoláljuk az értéket és a deriváltat,
4 szabadságfokra van szükségünk.
Ezt egy másodfokú $vc(w)$-vel el tudjuk érni,
hisz az $vc(r)'(t)$ integrálásakor választott konstans is egy szabadsági fok.
Így $vc(r)$ végül ötödfokú lesz, 2-vel több, mint egy ugyanennyi szabadságfokkal rendelkező hagyományos görbe.

Az önálló laboratóriumom során implementáltam az interpolálást Farouki könyve @pythag alapján,
bernstein alakban.
A programhoz készült egy interaktív vizuális interface,
ami mozgatható Bézier kontrollpontok alapján rajzol le egy PH síkgörbét,
az ahhoz tartozó valódi kontrollpoligont,
illetve egy offset görbét.


#figure(
  image("images/phplanar.png", width: 70%),
  caption: []
)

= PH térgörbék
A térgörbék pitagoraszi feltétele
$
  x'(t)^2 + y'(t)^2 + z'(t)^2 = sigma(t)^2
$

Pitagoraszi polinomnégyeseket 4 tetszőleges polinommal lehet generálni
$
  x'(t) &= m^2(t) + n^2(t) - p^2(t) - q^2(t) \
  y'(t) &= 2[m(t)q(t) + n(t)p(t)] \
  z'(t) &= 2[n(t)q(t) - m(t)p(t)] \
  sigma(t) &= m^2(t) + n^2(t) + p^2(t) + q^2(t)
$

Míg a síkbeli PH görbéket komplex számokkal lehet elegánsan reprezentálni,
a térbelieket kvaterniókkal
$
  cal(A)(t) = m(t) + n(t)vc(i) + p(t)vc(j) + q(t)vc(k) \
  vc(r)'(t) = cal(A)(t)vc(i)cal(A)^*(t)
$

== Interpoláció
Térgörbe esetén a polinomiális parametrikus sebességből még nem következik a racionális offset.
Ahhoz egy racionális "Frenet keret" kell,
ami az egységhosszúságú irányvektoron kívül tartalmazza az egyszéghosszúságú normál és binormál vektort.
Ez teljesül a PH térgörbék egy speciális alosztályára, a hélix (spirál) alakú PH görbékre.

Önálló laboratóriumom alatt egy PH térgörbe demot is készítettem.

#figure(
  image("images/phspatial.png", width: 70%),
  caption: []
)

#chapter[PN felületek]

"Pythagorean-Normal" (PN) felületnek hívjuk azokat az $vc(x)(u, v)$ polinomiális/racionális parametrikus felületeket,
melyekre teljesül, hogy az alábbi kifejezésben
$
  lr(|| dd(vc(x), u) times dd(vc(x), v) ||)^2 = sigma^2
$
$sigma$ polinomiális/racionális.

Ez azt jelenti, hogy az egységhosszúságú normálvektor is racionális
$
  vc(n)(u, v) = (dd(vc(x), u) times dd(vc(x), v)) / sigma
$
Tehát az offsetek is azok
$
  vc(x)_d(u, v) = vc(x) + d vc(n)
$

= PN felületek kutatása

A PH sík- és térgörbékkel ellentétben,
a PN felületek generálására nem létezik egyszerű, általános módszer.
A téma aktív kutatás alatt áll, számos különböző megközelítés áll versenyben.

Ezek közül pár fontosabb:

- *PN felület mint érintősíkok burkolófelülete @pottman*\
  Ez volt az első javaslat PN felületek előállítására.
  A felületek duális reprezentációját használja, Bézier alakban.

- *Másodfokú háromszögalapú Bézier foltok @bezierpatch*\
  A Bézier foltok (Bézier-patch) már korábban is elterjedtek voltak a számítógépes grafikában.
  Ez a cikk bemutatja, hogy a síkba torzítás nélkül nem lapítható (non-developable)
  Bézier foltok mind PN felületek, és ad egy algoritmust az ofszetjeik kiszámítására.

- *Polinomiális PN felületek kvaterniókkal @quaternion*\
  A duális reprezentációt használó módszerek hátránya, hogy a polinomiálisnál bonyolultabb racionális felületeket állítanak elő.
  Az itteni megoldás a PH térgörbék mintájára, kvaterniók használatával generál polinomiális PN felületeket.

- *PN interpoláció a duális tér izotróp modelljével @smooth*\
  A cikk módszert ad téglalaprácsban megadott pont-normálvektor adatok interpolálására.
  A duális reprezentációra építve bevezeti az izotróp teret,
  ami kényelmesebb interpolálást tesz lehetővé.

- *Polinomiális és lokális módszer $bold(upright(G^1))$ folytonosságú PN splinera @direct*\
  Háromszögháló alapú PN interpoláció számos kedvező tulajdonsággal.
  
- *PN minimálfelületek @minimal*\
  Izoterm paraméterezésű minimálfelületek előállítása pitagoraszi komplex polinomhármasokkal,
  melyek rendelkeznek a PN tulajdonsággal.

A továbbiakban Lávička et al. módszerét (izotróp tér) fogom feldolgozni.

#chapter[PN interpoláció $C^1$ folytonossággal]
Lávičkáék módszere egy pont-normálvektor párosokból álló négyzetrács alakú hálózatból indul ki.
Eredményül pedig négyzet alakú felület-foltok (patch) hálózatát adja,
melyek interpolálnak a pontok és normálvektorok között,
illetve egymáshoz $C^1$ folytonossággal illeszkednek.

A kreálandó felületet $vc(x)(u, v)$-nek hívjuk,
illetve a tömörebb szintaxis érdekében az ($u, v$) párost egy vektorba összevonva $vc(x)(vc(s))$-nek.

A folyamat tömör vázlata:
- az adatpontokat transzformáljuk a duális térbe, majd onnan az izotróp térbe
- az izotróp térben interpoláljuk az adatpontokat
- a kapott felületet visszatranszformáljuk a primális térbe

#import "@preview/fletcher:0.5.7" as fletcher: diagram, node, edge

#align(center)[
  #diagram(
    node-stroke: 1pt,
    node-corner-radius: 4pt,
    node-outset: 5pt,
    spacing: 7em,
    node((0,0), [
      Primális tér

      felület: $vc(x)$
    ]),
    edge([adatpontok], "-|>", bend: 40deg),
    edge([felület], "<|-", bend: -40deg),
    node((1,0), [
      Duális tér

      felület: $vc(b) = (vc(n), h)$
    ]),
    edge([adatpontok], "-|>", bend: 40deg),
    edge([felület], "<|-", bend: -40deg),
    node((2,0), [
      Izotróp tér

      felület: $vc(y)$
    ]),
  )
]

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

#figure(
  image("images/blaschke.png", width: 60%),
  caption: [vetítés a Blaschke henger és izotróp tér között @smooth]
)

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
Ha $vc(a)_(i, j)$-nek csak egy szomszédja van, vegyük a köztük lévő vektort
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
a deriváltak is hasonlóan egyszerűek
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

= Visszatranszformálás, az eredmény fokszáma
Az $vc(y)$ izotróp térből primális térbe transzformálásához szükséges műveleteket
a fejezet korábbi pontjaiban már ismertettük.
// TODO: hivatkozás?
Viszont gyakorlati megfontolások miatt érdemes ezeket tovább alakítani,
hogy a módszert végrehajtó programunk hatékonyabban tudjon futni.

A legfontosabb szempont a transzformálás során előforduló polinomok fokszámainak minimalizálása.
Egy kétváltozós polinom tagjainak száma négyzetesen arányos a fokszámmal,
a kiértékelés komplexitása köbösen (tagonként $O(n)$),
a polinomok közti szorzásé pedig már negyedik hatvánnyal (minden tagot minden taggal).

A racionális függvényeket reprezentáló kód jelenlegi formájában nem tudja a közös osztókat tartalmazó
számlálót és nevezőt egyszerűsíteni, így azok felhalmozódnak a számolás során.
Ez egyrészt egy szükségtelenül magas fokszámú reprezentációt ad a felületről,
amit több ideig tart kiértékelni.
Erre megoldás lehetne egy kódbeli implementáció a racionális függvények egyszerűsítésére.
Másrészt pedig magát a transzformációt is jelentősen lelassítja,
ha felesleges műveleteket kell elvégezni.
Ez még akkor is így lenne, ha minden lépés után elvégeznénk egy egyszerűsítést,
hisz az egyszerűsítésnek is van költsége.

A megvalósítás egy korábbi iterációjában a létrejött felület 56-od fokú lett,
és a programnak több órán át tartott kiszámolni
(érdekes módon a kiértékelés ehhez képest elenyészően kevés ideig tartott).
Szerencsére a felület "valódi" fokszáma ennél általános esetben is sokkal alacsonyabb.

Ez azt jelenti, hogy ha visszatranszformálást először "kézzel" átalakítjuk,
észreveszünk és kihasználunk néhány azonosságot,
akkor sokkal hatékonyabb kódot tudunk írni.

// Vegyük tehát $vc(x)$ 4.2-es pontban leírt képletét
// $
// vc(x) = h vc(n) + dd(vc(n), vc(s)) (dd(vc(n), vc(s))^T dd(vc(n), vc(s)))^(-1) dd(h, vc(s))^T \
// $
Vegyük $vc(y)$ transzformációját a Blaschke-hengerre
#centered_split(distance: 50%,
  $
    vc(n) = 1 / (1 + y_x^2 + y_y^2) vec(2 y_x, 2 y_y, -1 + y_x^2 + y_y^2)
  $,
  $
    h = (2 y_z) / (1 + y_x^2 + y_y^2)
  $
)
Adjunk külön nevet a számlálóknak és nevezőknek
#centered_split(distance: 50%,
  $
    vc(n) = vc(m) / q
  $,
  $
    h = k / q
  $
)
Ezzel $vc(x)$ első komponensét le is tudhatjuk
$
  vc(x) = h vc(n) + nabla = (k vc(m)) / q^2 + nabla
$
// A deriváltakat szorzatként fejezzük ki:
// #centered_split(distance: 30%,
//   $
//   dd(vc(n), vc(s)) = dd(vc(n), vc(y)) dd(vc(y), vc(s)) \
//   $,
//   $
//   dd(h, vc(s)) = dd(h, vc(y)) dd(vc(y), vc(s)) \
//   $,
// )
A deriváltak
#centered_split(distance: 30%,
  $
    dd(vc(n), vc(y)) = 2 / q^2 M\
  $,
  $
    dd(h, vc(y)) = 2 / q^2 vr(k)\
  $
)
Ahol
$
  M = mat(1 - y_x^2 + y_y^2,         -2y_x y_y;
                  -2 y_x y_y, 1 + y_x^2 - y_y^2;
                       2y_x,              2y_y;) \
  vr(k) = mat(-2 y_x y_z, -2y_y y_z, 1)
$
Az $y_z$ szerinti derivált oszlopát levágtuk $M$-ről, hisz az mind $0$.

A második komponens
$
nabla = dd(vc(n), vc(s)) (dd(vc(n), vc(s))^T dd(vc(n), vc(s)))^(-1) dd(h, vc(s))^T \
$
Tudjuk, hogy
#centered_split(distance: 40%,
  $
  dd(vc(n), vc(s)) = dd(vc(n), vc(y)) dd(vc(y), vc(s)) = 2 / q^2 M dd(vc(y), vc(s)) \
  $,
  $
  dd(h, vc(s)) = dd(h, vc(y)) dd(vc(y), vc(s)) = 2 / q^2 vr(k) dd(vc(y), vc(s)) \
  $,
)
Így
$
nabla = 4 / q^4 M dd(vc(y), vc(s))
  (4 / q^4 dd(vc(y), vc(s))^T M^T M dd(vc(y), vc(s)))^(-1) dd(vc(y), vc(s))^T vr(k)^T
$
Vegyük észre, hogy a $4 / q^4$ tagok kiejtik egymást.

$M$ két oszlopa (az $u$ illetve $v$ szerinti deriválthoz tartozó részek) mindig merőleges egymásra.
Belátható, hogy
$
M^T M = q^2 mat(1, 0; 0, 1)
$
#let Yxy = $Y_(x y)$
Bontsuk szét $dd(vc(y), vc(s))$-t
$
dd(vc(y), vc(s)) = vec(Yxy, vr(y)_z)
$
És rendezzünk tovább
$
nabla =1 / q^2 M Yxy (Yxy^T Yxy)^(-1) vec(Yxy, vr(y)_z)^T vr(k)^T
$
$
nabla = 1 / q^2 M (Yxy^(-1))^T vec(Yxy, vr(y)_z)^T vr(k)^T
$
$
nabla = 1 / q^2 M (vr(k) vec(Yxy, vr(y)_z) Yxy^(-1))^T
$
Bontsuk szét $vr(k)$-t is
$
vr(k) = mat(vr(k)_(x y), 1)
$
$
nabla = 1 / q^2 M (vr(k)_(x y) + vr(y)_z Yxy^(-1))^T
$
$Y_(x y)$ inverzének is vegyük külön a számlálóját és nevezőjét
$
Yxy^(-1) = 1 / abs(Yxy) Y'_(x y)
$
$
nabla = 1 / (q^2 abs(Yxy)) M (abs(Yxy) vr(k)_(x y) + vr(y)_z Y'_(x y))^T
$

Ezzel az egyszerűsítéssel csak 17-edfokú az eredmény,
a számítás pár másodperc alatt lefut.
Valójában még ez az alak sem optimális még,
egy CAS rendszer a kapott függvényt le tudja egyszerűsíteni 11-edfokúra.

#chapter[Implementációs részletek]
A módszert implementáló kódot C++-ban írtam,
a CMake build toolt használtam fordításkor.
A vektorokkal, mátrixokkal való számoláshoz az Eigen könyvtárat használtam.

= Polinom osztály
A polinomok szimbolikus reprezentációjához és manipulációjához létrehoztam a `Polynomial` osztályt.

== Változók
Ahelyett, hogy egy polinom együtthatóit pozícionálisan határoznánk meg
(tehát mondjuk egy `std::vector` $n$-edik eleme felel meg egy névtelen változó $n$-edik hatványához tartozó együtthatónak)
a polinomunkat egymástól megkülönböztetett szimbolikus változókkal fejezzük ki.
A változókat a `Variable` osztály reprezentálja.
Ez a háttérben nem több, mint egy ( `static` ) számláló, ami egyedi azonosítót ad minden változó példánynak.

A változók megkülönböztetése lehetővé teszi,
hogy műveleteket végezzünk el különböző paraméterekben értendő polinomok között anélkül,
hogy a paraméterek nem kívánt módon "összemosódjanak".
Így például össze tudunk adni/szorozni két egyváltozós polinomot úgy,
hogy az eredmény kétváltozós legyen
$
  f(u)g(v) = h(u, v)
$

== Struktúra
A `Variable` és `Polynomial` osztályok közti lépés a `PowerPermutation` osztály,
ami változók hatványpermutációját reprezentálja, például $x^2 y^3 z$.
A `PowerPermutation` hátterében egy `std::unordered_map` áll,
ami változókhoz rendel pozitív egész számokat ( `unsigned int` ).
Így egy `PowerPermutation` példány tetszőlegesen sok változóból állhat.
Az egyértelműség érdekében, ha egy változóhoz $0$-t rendelnénk, akkor azt az elemet töröljük
(mondjuk $x^2 y^0 = x^2$). A konstans permutációt (minden hatvány $0$) egy üres map jelöli.

A `Polynomial` adatstruktúrája szintén egy `std::unordered_map`,
ami `PowerPermutation`-ökhöz rendel együtthatókat.
A `PowerPermutation` - `coeff_type` párokra a kódban `term`-ként (tag) hivatkozok.

== Konstrukciós szintaxis
A `Variable`, `PowerPermutation` és `Polynomial` osztályok között definiálva van operator overloading
bizonyos aritmetikai operációkra,
amik lehetővé teszik a polinomok konstruálását egy inuitív és ismerős szintaxissal.
Erre a fontosabb példák

- `operator^(Variable, unsigned int)` $=>$ `PowerPermutation` (hatványozás)
- `operator*(Variable, Variable)` $=>$ `PowerPermutation`
- `operator*(PowerPermutation, PowerPermutation)` $=>$ `PowerPermutation`
- `operator*(coeff_type, PowerPermutation)` $=>$ `Polynomial`
- `operator+(PowerPermutation, PowerPermutation)` $=>$ `Polynomial`
- `operator+(Polynomial, Polynomial)` $=>$ `Polynomial`
- $dots$

Egy példa polinom-konstrukció
```cpp
  Variable x, y;
  Polynomial<double> p = (x^3) - 2.0 * x * (y^2) + 1.0
```

A C++ szintaxisából adódó limitáció, hogy a szorzásjelet nem lehet elhagyni,
illetve az operátor precedencia miatt a hatványozást szimbolizáló `^` jel használatatakor zárójelezni kell.

// TODO: kicsit sok a szám
== Funkcionalitás

A `Polynomial` osztályra az alábbi függvények definiáltak:

*Aritmetikai műveletek*
- összeadás
- kivonás
- szorzás

*Kiértékelés* \
Az `evaluate` függvény változó-érték párosok listáját várja
(pontosabban `std::initializer_list` )
majd tagonként, a `PowerPermutation`-ökbe behelyettesítve értékel ki.
Például
```cpp
  double result = p.evaluate<double>({{x, 3.0}, {y, -1.0}});
```

*Derivált* \
A `derivative` függvény egy változót vár, majd az adott változó szerinti deriváltat adja vissza.
Van egy alternatív verziója, ami változók listáját fogadja el,
majd a deriváltakat egy ennek megfelelő `std::vector`-ban adja vissza.

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
(pl. float, double, complex, vektor).
Nagyban növeli a rugalmasságot, hogy az együtthatók és a bemeneti paraméter külön vannak templatelve.
Így például lehet polinomunk ami double típusú paramétert kap,
de az együtthatói (így végül a `result_type`-ja is) vektor típusúak.

És mivel az `input_type`-ot csak az `evaluate` meghívásakor kell meghatározni,
ugyanazt a `Polynomial` példányt különböző típusú paraméterekkel ki lehet értékelni.

Egy kifejezetten érdekes és hasznos következménye a template-es működésnek az,
hogy két polinomot képesek vagyunk komponálni úgy, hogy az egyiket kiértékeljük a másikban.

== Racionális függvények
A `Polynomial` mintájára létrehoztam a `Rational` osztályt a racionális függvények reprezentálására.
Az osztály 2 `Polynomial`-ból áll, számláló és nevező.
A `Polynomial`-hoz hasonlóan definiáltak rajta az aritmetikai műveletek (osztással bővítve),
a kiértékelés, és a deriválás.

== Tesztek
A `Variable`, `PowerPermutation`, `Polynomial` és `Rational` osztályok működésének ellenőrzéséhez írtam unit-teszteket,
a GoogleTest könyvtárral.

= grid
A feldolgozandó adatok és kiszámolt köztes értékek kezelésére létrehoztam a templatelt `grid` osztályt,
ami egy $n times m$-es kétdimenziós tömböt reprezentál.
A `grid`-et a C++23-ban megjelent többdimenziós subscript operátorral ( `operator[]` ) kényelmesen meg lehet indexelni.

A `grid` egy $n times m$ nagyságú `std::vector`-t használ a háttérben,
iterátora az `std::vector` iterátora.

= range2d
Az egymásba ágyazott ciklusok elkerülése végett létrehoztam a `range2d` osztályt,
illetve ennek iterátorát, az `index2d`-t.
Az előbbi egy $n$ és $m$ számpár ( `size_t` ),
az utóbbi egy $i$, $j$ pár, illetve egy harmadik $m$ szám.

Az `index2d` inkrementálás operátora ( `operator++` ) úgy növeli $i$-t és $j$-t,
hogy végiglépegessen egy $m$ hosszú sorokból álló kétdimenziós tömb indexein.
A dereferálás operátora ( `operator*` ) pedig visszaadja $i$-t és $j$-t.

Így az alábbi szintaxissal végig lehet iterálni egy $n times m$-es tömb indexein

```cpp
  for (auto [i, j] : range2d{n, m}) {
    ...
  }
```

= Megjelenítés
A program a létrejött racionális patcheket kiértékeli $n times n$ darab ($u$, $v$) értékpárban,
a pontokat egy `grid`-ben tárolja.
Ezután a felületeteket exportálja a népszerű _obj_ fájlformátumban.
A kiírás történhet normálvektorokkal együtt, vagy nélkülük.
Lehetséges továbbá több patchet egyszerre, ugyanabba a fájlba írni.
Ez esetben a program a patcheket külön group-okba rakja a fájlon belül.
Az obj fájlok megjelenítésére sok alkalmazás képes, én a ParaView-t használtam.

#chapter[Eredmények]

= Szabad paraméterek
Már korábban is említettük, hogy a módszer elvi alapjai nem határoznak meg egy egyedi felületet bizonyos adatpontokra.
A folyamat során hozhatunk pár szabad döntést, amik befolyásolják az eredményt.
Ezek az eredmények mind megfelelnek bizonyos alapvető elvárásoknak
(a felület folytonos, interpolál a megadott pontok és normálvektorok között)
de adott esetben komolyabb esztétikai hibák és nem kívánt anomáliák jelenhetnek meg.

A legfőbb ezek közül az amikor élek jelennek meg a felületen,
a felület egy vonal mentén hirtelen megfordul.
Ilyen lehet egyrészt a patchek találkozásánál
(maguk a patchek külön-külön simák, de a határokon ellentétes irányba tartanak)
de akár a patcheken belül is.

#figure(
  image("images/isotropic.png", width: 70%),
  caption: [az izotróp térben sima a felület, de a primális térben nem feltétlenül]
)

= Végponti deriváltak

A legkézenfekvőbb szabad paraméter az izotróp térben megszabott végponti deriváltak nagysága.
Minél nagyobb a szorzó, a felület annál kaotikusabban viselkedik.
Extrém mértékben kitér, sokszor önmagára hajlik.
Ezzel szemben a kicsi szorzó hatására a felület "merev" lesz.
Az adatpontok környékén lapos, a pont-normálvektor pár által meghatározott síkot közelíti.
A lapok találkozásánál pedig szintén élek alakulnak ki.

#figure(
  image("images/highmul.png", width: 70%),
  caption: [nagy derivált az izotróp térben]
)

#figure(
  image("images/lowmul.png", width: 70%),
  caption: [kicsi derivált]
)

Nem muszáj ragaszkodnunk az interpolálandó deriváltak irányát meghatározó heurisztikához sem.
A vektorok egy adott síkon belül (melynek normálvektora a folyamat során kiszámolt $vc(v)$)
bármerre mutathatnak.
És bár a heurisztikánk az izotróp térben nagyon szép, "egyenes" határgörbékhez vezet,
ez a tulajdonság nem feltétlenül marad meg visszatranszformálás után.
Előfordulhat, hogy egy patch túlságosan be-, vagy kilóg a sarokpontok közti "keretből".
Az itt használt módszernek egy összetettebb alternatívája lehetne például az,
hogy a derivált-heurisztikát még a primális térben alkalmazzuk,
a deriváltakat transzformáljuk az izotróp térbe, majd ezeket vetítjük le a megfelelő síkra.

= Adatpontok előzetes transzformálása
A módszer egy nem kifejezetten szerencsés tulajdonsága az,
hogy nem lineáris (illetve pontosabban affin).
Ez alatt az értendő, hogy amennyiben az adatpontokat eltoljuk, forgatjuk, vagy skálázzuk,
a létrejövő felület nem az eredeti hasonlóan transzformált verziója lesz,
attól drasztikus mértékben eltérhet.

Ez egy újabb dimenziót ad a felület igazításához.
Adatpontjainkat áttranszformálhatjuk a módszer végrehajtása előtt,
majd az eredményt visszatranszformálhatjuk, hogy egy új interpolációt kapjunk.
Ezzel a lehetőséggel bizonyos esetben szükséges is élnünk,
hogy felületünk megfeleljen az esztétikai elvárásainknak.

== Eltolás
Ha az adatpontjaink túl távol vannak az origótól,
a patchek között élek alakulnak ki.
Ezek az élek látványosan eltérnek a patchen belüli élektől,
amiket például a nagy izotróp végponti deriváltak okoznak.
Itt a patchek tesznek egy kanyart a határgörbe közelében, és "fordítva" érnek be.

Érdemes ezért az adatpontokat úgy eltolni, hogy az origó a közepükön legyen.

#figure(
  image("images/offcenter.png", width: 70%),
  caption: [az origó a sötétkék sarkon van]
)

== Skálázás
Még egy egyszerű transzformáció az arányos skálázás,
ergo az összes adatpontot beszorozzuk ugyanazzal a számmal
(a normálvektorokat békén hagyjuk, azok mindenképp egység hosszúságúak).
Egy magasabb szorzó "kifeszíti" a felületet.
Az éleket ki tudja simítani,
de cserébe a felület szélei "behorpadnak",
a határgörbék egyre inkább kitérnek az origó irányába.

A lefele skálázás szépen kiegyenesíti a határgörbéket,
azonban a patchek határai környékén éleket alakíthat ki.

#figure(
  image("images/highscale.png", width: 70%),
  caption: [a nagy deriváltat nagy skálázással kompenzáljuk]
)

#figure(
  image("images/lowscale.png", width: 70%),
  caption: [jelentősen csökkentettük a skálázást, az élek egy kicsit rosszabbak lettek]
)

// == Forgatás

= Végeredmény megfelelő paraméterekkel
#figure(
  image("images/ideal.png", width: 70%),
  caption: []
)

#chapter[Tanulságok]
#todo("megírni")
= Módszer megítélése

= További kutatás

#set heading(offset: 0)
#bibliography("works.yml", style: "elsevier-vancouver", full: true)
