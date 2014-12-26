#include <phpcpp.h>
#include <iostream>
#include <string>
#include <sstream>
#include <pcrecpp.h>

enum class Gender { M, F, S, ignore }; // masculine, feminine, neuter

struct Pattern
{
    Gender gender;
    std::string regex; // nominative
    std::string cases[13]; // singular case 2..7, plural cases 1..7
};

struct RegexPattern
{
    pcrecpp::RE regex;
    Pattern patern;
};

struct Exception
{
    std::string nominative;
    std::string stem;
    std::string accusative;
};

class Inflection : public Php::Base
{
private:
    const Pattern patterns[181] = {
        {Gender::M, "-ký", {"kého", "kému", "ký/kého", "ký", "kém", "kým", "ké/cí", "kých", "kým", "ké", "ké/cí", "kých", "kými"}},
        // modrý
        {Gender::M, "-rý", {"rého", "rému", "rý/rého", "rý", "rém", "rým", "ré/ří", "rých", "rým", "ré", "ré/ří", "rých", "rými"}},
        // jednodychý
        {Gender::M, "-chý", {"chého", "chému", "chý/chého", "chý", "chém", "chým", "ché/ší", "chých", "chým", "ché", "ché/ší", "chých", "chými"}},
        // strohý
        {Gender::M, "-hý", {"hého", "hému", "hý/hého", "hý", "hém", "hým", "hé/zí", "hých", "hým", "hé", "hé/zí", "hých", "hými"}},
        // jedlý
        {Gender::M, "-ý", {"ého", "ému", "ý/ého", "ý", "ém", "ým", "é/í", "ých", "ým", "é", "é/í", "ých", "ými"}},
        // spící
        {Gender::M, "-([aeěií])cí", {"0cího", "0címu", "0cí/0cího", "0cí", "0cím", "0cím", "0cí", "0cích", "0cím", "0cí", "0cí", "0cích", "0cími"}},
        {Gender::F, "-([aeěií])cí", {"0cí", "0cí", "0cí", "0cí", "0cí", "0cí", "0cí", "0cích", "0cím", "0cí", "0cí", "0cích", "0cími"}},
        {Gender::S, "-([aeěií])cí", {"0cího", "0címu", "0cí/0cího", "0cí", "0cím", "0cím", "0cí", "0cích", "0cím", "0cí", "0cí", "0cích", "0cími"}},
        // svatební
        {Gender::M, "-([bcčdhklmnprsštvzž])ní", {"0ního", "0nímu", "0ní/0ního", "0ní", "0ním", "0ním", "0ní", "0ních", "0ním", "0ní", "0ní", "0ních", "0ními"}},
        {Gender::F, "-([bcčdhklmnprsštvzž])ní", {"0ní", "0ní", "0ní", "0ní", "0ní", "0ní", "0ní", "0ních", "0ním", "0ní", "0ní", "0ních", "0ními"}},
        {Gender::S, "-([bcčdhklmnprsštvzž])ní", {"0ního", "0nímu", "0ní/0ního", "0ní", "0ním", "0ním", "0ní", "0ních", "0ním", "0ní", "0ní", "0ních", "0ními"}},
        // držitel
        {Gender::M, "-([i])tel", {"0tele", "0teli", "0tele", "0tel", "0teli", "0telem", "0telé", "0telů", "0telům", "0tele", "0telé", "0telích", "0teli"}},
        // přítel
        {Gender::M, "-([í])tel", {"0tele", "0teli", "0tele", "0tel", "0teli", "0telem", "átelé", "áteli", "átelům", "átele", "átelé", "átelích", "áteli"}},
        // malé
        {Gender::S, "-é", {"ého", "ému", "é", "é", "ém", "ým", "á", "ých", "ým", "á", "á", "ých", "ými"}},
        // malá
        {Gender::F, "-á", {"é", "é", "ou", "á", "é", "ou", "é", "ých", "ým", "é", "é", "ých", "ými"}},

        {Gender::ignore, "já", {"mne", "mně", "mne/mě", "já", "mně", "mnou", "my", "nás", "nám", "nás", "my", "nás", "námi"}},
        {Gender::ignore, "ty", {"tebe", "tobě", "tě/tebe", "ty", "tobě", "tebou", "vy", "vás", "vám", "vás", "vy", "vás", "vámi"}},
        {Gender::ignore, "my", {"", "", "", "", "", "", "my", "nás", "nám", "nás", "my", "nás", "námi"}},
        {Gender::ignore, "vy", {"", "", "", "", "", "", "vy", "vás", "vám", "vás", "vy", "vás", "vámi"}},
        {Gender::M, "on", {"něho", "mu/jemu/němu", "ho/jej", "on", "něm", "ním", "oni", "nich", "nim", "je", "oni", "nich", "jimi/nimi"}},
        {Gender::M, "oni", {"", "", "", "", "", "", "oni", "nich", "nim", "je", "oni", "nich", "jimi/nimi"}},
        {Gender::F, "ony", {"", "", "", "", "", "", "ony", "nich", "nim", "je", "ony", "nich", "jimi/nimi"}},
        {Gender::S, "ono", {"něho", "mu/jemu/němu", "ho/jej", "ono", "něm", "ním", "ona", "nich", "nim", "je", "ony", "nich", "jimi/nimi"}},
        {Gender::F, "ona", {"ní", "ní", "ji", "ona", "ní", "ní", "ony", "nich", "nim", "je", "ony", "nich", "jimi/nimi"}},
        {Gender::M, "ten", {"toho", "tomu", "toho", "ten", "tom", "tím", "ti", "těch", "těm", "ty", "ti", "těch", "těmi"}},
        {Gender::F, "ta", {"té", "té", "tu", "ta", "té", "tou", "ty", "těch", "těm", "ty", "ty", "těch", "těmi"}},
        {Gender::S, "to", {"toho", "tomu", "toho", "to", "tom", "tím", "ta", "těch", "těm", "ta", "ta", "těch", "těmi"}},

        // přivlastňovací zájmena
        {Gender::M, "můj", {"mého", "mému", "mého", "můj", "mém", "mým", "mí", "mých", "mým", "mé", "mí", "mých", "mými"}},
        {Gender::F, "má", {"mé", "mé", "mou", "má", "mé", "mou", "mé", "mých", "mým", "mé", "mé", "mých", "mými"}},
        {Gender::F, "moje", {"mé", "mé", "mou", "má", "mé", "mou", "moje", "mých", "mým", "mé", "mé", "mých", "mými"}},
        {Gender::S, "mé", {"mého", "mému", "mé", "moje", "mém", "mým", "mé", "mých", "mým", "má", "má", "mých", "mými"}},
        {Gender::S, "moje", {"mého", "mému", "moje", "moje", "mém", "mým", "moje", "mých", "mým", "má", "má", "mých", "mými"}},

        {Gender::M, "tvůj", {"tvého", "tvému", "tvého", "tvůj", "tvém", "tvým", "tví", "tvých", "tvým", "tvé", "tví", "tvých", "tvými"}},
        {Gender::F, "tvá", {"tvé", "tvé", "tvou", "tvá", "tvé", "tvou", "tvé", "tvých", "tvým", "tvé", "tvé", "tvých", "tvými"}},
        {Gender::F, "tvoje", {"tvé", "tvé", "tvou", "tvá", "tvé", "tvou", "tvé", "tvých", "tvým", "tvé", "tvé", "tvých", "tvými"}},
        {Gender::S, "tvé", {"tvého", "tvému", "tvého", "tvůj", "tvém", "tvým", "tvá", "tvých", "tvým", "tvé", "tvá", "tvých", "tvými"}},
        {Gender::S, "tvoje", {"tvého", "tvému", "tvého", "tvůj", "tvém", "tvým", "tvá", "tvých", "tvým", "tvé", "tvá", "tvých", "tvými"}},

        {Gender::M, "náš", {"našeho", "našemu", "našeho", "náš", "našem", "našim", "naši", "našich", "našim", "naše", "naši", "našich", "našimi"}},
        {Gender::F, "naše", {"naší", "naší", "naši", "naše", "naší", "naší", "naše", "našich", "našim", "naše", "naše", "našich", "našimi"}},
        {Gender::S, "naše", {"našeho", "našemu", "našeho", "naše", "našem", "našim", "naše", "našich", "našim", "naše", "naše", "našich", "našimi"}},

        {Gender::M, "váš", {"vašeho", "vašemu", "vašeho", "váš", "vašem", "vašim", "vaši", "vašich", "vašim", "vaše", "vaši", "vašich", "vašimi"}},
        {Gender::F, "vaše", {"vaší", "vaší", "vaši", "vaše", "vaší", "vaší", "vaše", "vašich", "vašim", "vaše", "vaše", "vašich", "vašimi"}},
        {Gender::S, "vaše", {"vašeho", "vašemu", "vašeho", "vaše", "vašem", "vašim", "vaše", "vašich", "vašim", "vaše", "vaše", "vašich", "vašimi"}},

        {Gender::M, "jeho", {"jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho"}},
        {Gender::F, "jeho", {"jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho"}},
        {Gender::S, "jeho", {"jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho", "jeho"}},

        {Gender::M, "její", {"jejího", "jejímu", "jejího", "její", "jejím", "jejím", "její", "jejích", "jejím", "její", "její", "jejích", "jejími"}},
        {Gender::S, "její", {"jejího", "jejímu", "jejího", "její", "jejím", "jejím", "její", "jejích", "jejím", "její", "její", "jejích", "jejími"}},
        {Gender::F, "její", {"její", "její", "její", "její", "její", "její", "její", "jejích", "jejím", "její", "její", "jejích", "jejími"}},

        {Gender::M, "jejich", {"jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich"}},
        {Gender::S, "jejich", {"jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich"}},
        {Gender::F, "jejich", {"jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich", "jejich"}},

        // výjimky (zvl. běžná slova)
        {Gender::M, "-bůh", {"boha", "bohu", "boha", "bože", "bohovi", "bohem", "bozi/bohové", "bohů", "bohům", "bohy", "bozi/bohové", "bozích", "bohy"}},
        {Gender::M, "-pan", {"pana", "panu", "pana", "pane", "panu", "panem", "páni/pánové", "pánů", "pánům", "pány", "páni/pánové", "pánech", "pány"}},
        {Gender::S, "moře", {"moře", "moři", "moře", "moře", "moři", "mořem", "moře", "moří", "mořím", "moře", "moře", "mořích", "moři"}},
        {Gender::ignore, "dveře", {"", "", "", "", "", "", "dveře", "dveří", "dveřím", "dveře", "dveře", "dveřích", "dveřmi"}},
        {Gender::ignore, "housle", {"", "", "", "", "", "", "housle", "houslí", "houslím", "housle", "housle", "houslích", "houslemi"}},
        {Gender::ignore, "šle", {"", "", "", "", "", "", "šle", "šlí", "šlím", "šle", "šle", "šlích", "šlemi"}},
        {Gender::ignore, "muka", {"", "", "", "", "", "", "muka", "muk", "mukám", "muka", "muka", "mukách", "mukami"}},
        {Gender::S, "ovoce", {"ovoce", "ovoci", "ovoce", "ovoce", "ovoci", "ovocem", "", "", "", "", "", "", ""}},
        {Gender::M, "humus", {"humusu", "humusu", "humus", "humuse", "humusu", "humusem", "humusy", "humusů", "humusům", "humusy", "humusy", "humusech", "humusy"}},
        {Gender::M, "-vztek", {"vzteku", "vzteku", "vztek", "vzteku", "vzteku", "vztekem", "vzteky", "vzteků", "vztekům", "vzteky", "vzteky", "vztecích", "vzteky"}},
        {Gender::M, "-dotek", {"doteku", "doteku", "dotek", "doteku", "doteku", "dotekem", "doteky", "doteků", "dotekům", "doteky", "doteky", "dotecích", "doteky"}},
        {Gender::F, "-hra", {"hry", "hře", "hru", "hro", "hře", "hrou", "hry", "her", "hrám", "hry", "hry", "hrách", "hrami"}},
        {Gender::M, "zeus", {"dia", "diovi", "dia", "die", "diovi", "diem", "diové", "diů", "diům", "", "diové", "", ""}},
        {Gender::F, "nikol", {"nikol", "nikol", "nikol", "nikol", "nikol", "nikol", "nikol", "nikol", "nikol", "nikol", "nikol", "nikol", "nikol"}},

        // číslovky
        {Gender::ignore, "-tdva", {"tidvou", "tidvoum", "tdva", "tdva", "tidvou", "tidvěmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-tdvě", {"tidvou", "tidvěma", "tdva", "tdva", "tidvou", "tidvěmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-ttři", {"titří", "titřem", "ttři", "ttři", "titřech", "titřemi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-tčtyři", {"tičtyřech", "tičtyřem", "tčtyři", "tčtyři", "tičtyřech", "tičtyřmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-tpět", {"tipěti", "tipěti", "tpět", "tpět", "tipěti", "tipěti", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-tšest", {"tišesti", "tišesti", "tšest", "tšest", "tišesti", "tišesti", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-tsedm", {"tisedmi", "tisedmi", "tsedm", "tsedm", "tisedmi", "tisedmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-tosm", {"tiosmi", "tiosmi", "tosm", "tosm", "tiosmi", "tiosmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-tdevět", {"tidevíti", "tidevíti", "tdevět", "tdevět", "tidevíti", "tidevíti", "", "", "", "", "", "", ""}},

        {Gender::F, "-jedna", {"jedné", "jedné", "jednu", "jedno", "jedné", "jednou", "", "", "", "", "", "", ""}},
        {Gender::M, "-jeden", {"jednoho", "jednomu", "jednoho", "jeden", "jednom", "jedním", "", "", "", "", "", "", ""}},
        {Gender::S, "-jedno", {"jednoho", "jednomu", "jednoho", "jedno", "jednom", "jedním", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-dva", {"dvou", "dvoum", "dva", "dva", "dvou", "dvěmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-dvě", {"dvou", "dvoum", "dva", "dva", "dvou", "dvěmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-tři", {"tří", "třem", "tři", "tři", "třech", "třemi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-čtyři", {"čtyřech", "čtyřem", "čtyři", "čtyři", "čtyřech", "čtyřmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-pět", {"pěti", "pěti", "pět", "pět", "pěti", "pěti", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-šest", {"šesti", "šesti", "šest", "šest", "šesti", "šesti", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-sedm", {"sedmi", "sedmi", "sedm", "sedm", "sedmi", "sedmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-osm", {"osmi", "osmi", "osm", "osm", "osmi", "osmi", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-devět", {"devíti", "devíti", "devět", "devět", "devíti", "devíti", "", "", "", "", "", "", ""}},

        {Gender::ignore, "deset", {"deseti", "deseti", "deset", "deset", "deseti", "deseti", "", "", "", "", "", "", ""}},

        {Gender::ignore, "-ná([cs])t", {"ná0ti", "ná0ti", "ná0t", "náct", "ná0ti", "ná0ti", "", "", "", "", "", "", ""}},

        {Gender::ignore, "-dvacet", {"dvaceti", "dvaceti", "dvacet", "dvacet", "dvaceti", "dvaceti", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-třicet", {"třiceti", "třiceti", "třicet", "třicet", "třiceti", "třiceti", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-čtyřicet", {"čtyřiceti", "čtyřiceti", "čtyřicet", "čtyřicet", "čtyřiceti", "čtyřiceti", "", "", "", "", "", "", ""}},
        {Gender::ignore, "-desát", {"desáti", "desáti", "desát", "desát", "desáti", "desáti", "", "", "", "", "", "", ""}},

        {Gender::M, "-([i])sta", {"0sty", "0stovi", "0stu", "0sto", "0stovi", "0stou", "0sté", "0stů", "0stům", "0sty", "0sté", "0stech", "0sty"}},
        {Gender::M, "-([o])sta", {"0sty", "0stovi", "0stu", "0sto", "0stovi", "0stou", "0stové", "0stů", "0stům", "0sty", "0sté", "0stech", "0sty"}},

        {Gender::M, "-předseda", {"předsedy", "předsedovi", "předsedu", "předsedo", "předsedovi", "předsedou", "předsedové", "předsedů", "předsedům", "předsedy", "předsedové", "předsedech", "předsedy"}},
        {Gender::M, "-srdce", {"srdce", "srdi", "sdrce", "srdce", "srdci", "srdcem", "srdce", "srdcí", "srdcím", "srdce", "srdce", "srdcích", "srdcemi"}},

        // žalobce
        {Gender::M, "-([db])ce", {"0ce", "0ci", "0ce", "0če", "0ci", "0cem", "0ci/0cové", "0ců", "0cům", "0ce", "0ci/0cové", "0cích", "0ci"}},
        // jev
        {Gender::M, "-([jň])ev", {"0evu", "0evu", "0ev", "0eve", "0evu", "0evem", "0evy", "0evů", "0evům", "0evy", "0evy", "0evech", "0evy"}},
        // lev
        {Gender::M, "-([lř])ev", {"0evu/0va", "0evu/0vovi", "0ev/0va", "0eve/0ve", "0evu/0vovi", "0evem/0vem", "0evy/0vové", "0evů/0vů", "0evům/0vům", "0evy/0vy", "0evy/0vové", "0evech/0vech", "0evy/0vy"}},
        // vůz
        {Gender::M, "-ů([lz])", {"o0u/o0a", "o0u/o0ovi", "ů0/o0a", "o0e", "o0u", "o0em", "o0y/o0ové", "o0ů", "o0ům", "o0y", "o0y/o0ové", "o0ech", "o0y"}},

        {Gender::M, "nůž", {"nože", "noži", "nůž", "noži", "noži", "nožem", "nože", "nožů", "nožům", "nože", "nože", "nožích", "noži"}},

        // clo
        {Gender::S, "-([bcčdghksštvzž])lo", {"0la", "0lu", "0lo", "0lo", "0lu", "0lem", "0la", "0el", "0lům", "0la", "0la", "0lech", "0ly"}},
        // ramínko
        {Gender::S, "-([bcčdnsštvzž])ko", {"0ka", "0ku", "0ko", "0ko", "0ku", "0kem", "0ka", "0ek", "0kům", "0ka", "0ka", "0cích/0kách", "0ky"}},
        // okno
        {Gender::S, "-([bcčdksštvzž])no", {"0na", "0nu", "0no", "0no", "0nu", "0nem", "0na", "0en", "0nům", "0na", "0na", "0nech/0nách", "0ny"}},
        // kolo
        {Gender::S, "-o", {"a", "u", "o", "o", "u", "em", "a", "", "ům", "a", "a", "ech", "y"}},
        // stavení
        {Gender::S, "-í", {"í", "í", "í", "í", "í", "ím", "í", "í", "ím", "í", "í", "ích", "ími"}},
        // děvče
        {Gender::S, "-([čďť])([e])", {"10te", "10ti", "10", "10", "10ti", "10tem", "1ata", "1at", "1atům", "1ata", "1ata", "1atech", "1aty"}},
        // veka
        {Gender::F, "-([aeiouyáéíóúý])ka", {"0ky", "0ce", "0ku", "0ko", "0ce", "0kou", "0ky", "0k", "0kám", "0ky", "0ky", "0kách", "0kami"}},
        // radka
        {Gender::F, "-ka", {"ky", "ce", "ku", "ko", "ce", "kou", "ky", "ek", "kám", "ky", "ky", "kách", "kami"}},
        // kra
        {Gender::F, "-([bdghkmnptvz])ra", {"0ry", "0ře", "0ru", "0ro", "0ře", "0rou", "0ry", "0er", "0rám", "0ry", "0ry", "0rách", "0rami"}},
        // dcera
        {Gender::F, "-ra", {"ry", "ře", "ru", "ro", "ře", "rou", "ry", "r", "rám", "ry", "ry", "rách", "rami"}},
        // lampa
        {Gender::F, "-([tdbnvmp])a", {"0y", "0ě", "0u", "0o", "0ě", "0ou", "0y", "0", "0ám", "0y", "0y", "0ách", "0ami"}},
        // střecha
        {Gender::F, "-cha", {"chy", "še", "chu", "cho", "še", "chou", "chy", "ch", "chám", "chy", "chy", "chách", "chami"}},
        // něha
        {Gender::F, "-([gh])a", {"0y", "ze", "0u", "0o", "ze", "0ou", "0y", "0", "0ám", "0y", "0y", "0ách", "0ami"}},
        // Soňa
        {Gender::F, "-ňa", {"ni", "ně", "ňou", "ňo", "ni", "ňou", "ně/ničky", "ň", "ňám", "ně/ničky", "ně/ničky", "ňách", "ňami"}},
        // Dáša
        {Gender::F, "-([šč])a", {"0i", "0e", "0u", "0o", "0e", "0ou", "0e/0i", "0", "0ám", "0e/0i", "0e/0i", "0ách", "0ami"}},
        // žena
        {Gender::F, "-a", {"y", "e", "u", "o", "e", "ou", "y", "", "ám", "y", "y", "ách", "ami"}},
        // píseň
        {Gender::F, "-eň", {"ně", "ni", "eň", "ni", "ni", "ní", "ně", "ní", "ním", "ně", "ně", "ních", "němi"}},
        // Třeboň
        {Gender::F, "-oň", {"oně", "oni", "oň", "oni", "oni", "oní", "oně", "oní", "oním", "oně", "oně", "oních", "oněmi"}},
        // beznaděj
        {Gender::F, "-([ě])j", {"0je", "0ji", "0j", "0ji", "0ji", "0jí", "0je", "0jí", "0jím", "0je", "0je", "0jích", "0jemi"}},
        // lahev
        {Gender::F, "-ev", {"ve", "vi", "ev", "vi", "vi", "ví", "ve", "ví", "vím", "ve", "ve", "vích", "vemi"}},
        // kytice
        {Gender::F, "-ice", {"ice", "ici", "ici", "ice", "ici", "icí", "ice", "ic", "icím", "ice", "ice", "icích", "icemi"}},
        // růže
        {Gender::F, "-e", {"e", "i", "i", "e", "i", "í", "e", "í", "ím", "e", "e", "ích", "emi"}},
        // epopej
        {Gender::F, "-([eaá])([jžň])", {"10e/10i", "10i", "10", "10i", "10i", "10í", "10e/10i", "10í", "10ím", "10e", "10e", "10ích", "10emi"}},
        // myš
        {Gender::F, "-([eayo])([š])", {"10e/10i", "10i", "10", "10i", "10i", "10í", "10e/10i", "10í", "10ím", "10e", "10e", "10ích", "10emi"}},
        // skříň
        {Gender::F, "-([íy])ň", {"0ně", "0ni", "0ň", "0ni", "0ni", "0ní", "0ně", "0ní", "0ním", "0ně", "0ně", "0ních", "0němi"}},
        // kolegyně
        // TODO verify ňe is ok
        {Gender::F, "-([íyý])ňe", {"0ně", "0ni", "0ň", "0ni", "0ni", "0ní", "0ně", "0ní", "0ním", "0ně", "0ně", "0ních", "0němi"}},
        // trať
        {Gender::F, "-([ťďž])", {"0e", "0i", "0", "0i", "0i", "0í", "0e", "0í", "0ím", "0e", "0e", "0ích", "0emi"}},
        // laboratoř
        {Gender::F, "-toř", {"toře", "toři", "toř", "toři", "toři", "toří", "toře", "toří", "tořím", "toře", "toře", "tořích", "tořemi"}},
        // step
        {Gender::F, "-ep", {"epi", "epi", "ep", "epi", "epi", "epí", "epi", "epí", "epím", "epi", "epi", "epích", "epmi"}},

        // kost
        {Gender::F, "-st", {"sti", "sti", "st", "sti", "sti", "stí", "sti", "stí", "stem", "sti", "sti", "stech", "stmi"}},

        {Gender::F, "ves", {"vsi", "vsi", "ves", "vsi", "vsi", "vsí", "vsi", "vsí", "vsem", "vsi", "vsi", "vsech", "vsemi"}},

        // Amadeus
        {Gender::M, "-([e])us", {"0a", "0u/0ovi", "0a", "0e", "0u/0ovi", "0em", "0ové", "0ů", "0ům", "0y", "0ové", "0ích", "0y"}},
        // Celsius
        {Gender::M, "-([i])us", {"0a", "0u/0ovi", "0a", "0e", "0u/0ovi", "0em", "0ové", "0ů", "0ům", "0usy", "0ové", "0ích", "0usy"}},
        // Denis
        {Gender::M, "-([i])s", {"0se", "0su/0sovi", "0se", "0se/0si", "0su/0sovi", "0sem", "0sy/0sové", "0sů", "0sům", "0sy", "0sy/0ové", "0ech", "0sy"}},

        {Gender::M, "výtrus", {"výtrusu", "výtrusu", "výtrus", "výtruse", "výtrusu", "výtrusem", "výtrusy", "výtrusů", "výtrusům", "výtrusy", "výtrusy", "výtrusech", "výtrusy"}},
        {Gender::M, "trus", {"trusu", "trusu", "trus", "truse", "trusu", "trusem", "trusy", "trusů", "trusům", "trusy", "trusy", "trusech", "trusy"}},

        // pokus
        {Gender::M, "-([aeioumpts])([lnmrktp])us", {"10u/10a", "10u/10ovi", "10us/10a", "10e", "10u/10ovi", "10em", "10y/10ové", "10ů", "10ům", "10y", "10y/10ové", "10ech", "10y"}},
        // útlum
        {Gender::S, "-([l])um", {"0a", "0u", "0um", "0um", "0u", "0em", "0a", "0", "0ům", "0a", "0a", "0ech", "0y"}},
        // publikum
        {Gender::S, "-([k])um", {"0a", "0u", "0um", "0um", "0u", "0em", "0a", "0", "0ům", "0a", "0a", "0cích", "0y"}},
        // medium
        {Gender::S, "-([i])um", {"0a", "0u", "0um", "0um", "0u", "0em", "0a", "0í", "0ům", "0a", "0a", "0iích", "0y"}},
        // rádio
        {Gender::S, "-io", {"0a", "0u", "0", "0", "0u", "0em", "0a", "0í", "0ům", "0a", "0a", "0iích", "0y"}},
        // bar
        {Gender::M, "-([aeiouyáéíóúý])r", {"0ru/0ra", "0ru/0rovi", "0r/0ra", "0re", "0ru/0rovi", "0rem", "0ry/0rové", "0rů", "0rům", "0ry", "0ry/0rové", "0rech", "0ry"}},
        // odběr
        {Gender::M, "-r", {"ru/ra", "ru/rovi", "r/ra", "ře", "ru/rovi", "rem", "ry/rové", "rů", "rům", "ry", "ry/rové", "rech", "ry"}},
        // kámen
        {Gender::M, "-([mnp])en", {"0enu/0ena", "0enu/0enovi", "0en/0na", "0ene", "0enu/0enovi", "0enem", "0eny/0enové", "0enů", "0enům", "0eny", "0eny/0enové", "0enech", "0eny"}},
        // hřeben
        {Gender::M, "-([bcčdstvz])en", {"0nu/0na", "0nu/0novi", "0en/0na", "0ne", "0nu/0novi", "0nem", "0ny/0nové", "0nů", "0nům", "0ny", "0ny/0nové", "0nech", "0ny"}},
        // vtip/pes
        {Gender::M, "-([dglmnpbtvzs])", {"0u/0a", "0u/0ovi", "0/0a", "0e", "0u/0ovi", "0em", "0y/0ové", "0ů", "0ům", "0y", "0y/0ové", "0ech", "0y"}},
        // reflex
        {Gender::M, "-([x])", {"0u/0e", "0u/0ovi", "0/0e", "0i", "0u/0ovi", "0em", "0y/0ové", "0ů", "0ům", "0y", "0y/0ové", "0ech", "0y"}},

        {Gender::M, "sek", {"seku/seka", "seku/sekovi", "sek/seka", "seku", "seku/sekovi", "sekem", "seky/sekové", "seků", "sekům", "seky", "seky/sekové", "secích", "seky"}},
        {Gender::M, "výsek", {"výseku/výseka", "výseku/výsekovi", "výsek/výseka", "výseku", "výseku/výsekovi", "výsekem", "výseky/výsekové", "výseků", "výsekům", "výseky", "výseky/výsekové", "výsecích", "výseky"}},
        {Gender::M, "zásek", {"záseku/záseka", "záseku/zásekovi", "zásek/záseka", "záseku", "záseku/zásekovi", "zásekem", "záseky/zásekové", "záseků", "zásekům", "záseky", "záseky/zásekové", "zásecích", "záseky"}},
        {Gender::M, "průsek", {"průseku/průseka", "průseku/průsekovi", "průsek/průseka", "průseku", "průseku/průsekovi", "průsekem", "průseky/průsekové", "průseků", "výsekům", "průseky", "průseky/průsekové", "průsecích", "průseky"}},

        // polibek
        {Gender::M, "-([cčšždnňmpbrstvz])ek", {"0ku/0ka", "0ku/0kovi", "0ek/0ka", "0ku", "0ku/0kovi", "0kem", "0ky/0kové", "0ků", "0kům", "0ky", "0ky/0kové", "0cích", "0ky"}},
        // tabák
        {Gender::M, "-([k])", {"0u/0a", "0u/0ovi", "0/0a", "0u", "0u/0ovi", "0em", "0y/0ové", "0ů", "0ům", "0y", "0y/0ové", "cích", "0y"}},
        // prach
        {Gender::M, "-ch", {"chu/cha", "chu/chovi", "ch/cha", "chu/cha", "chu/chovi", "chem", "chy/chové", "chů", "chům", "chy", "chy/chové", "ších", "chy"}},
        // dosah
        {Gender::M, "-([h])", {"0u/0a", "0u/0ovi", "0/0a", "0u", "0u/0ovi", "0em", "0y/0ové", "0ů", "0ům", "0y", "0y/0ové", "zích", "0y"}},
        // duben
        {Gender::M, "-e([mnz])", {"0u/0a", "0u/0ovi", "e0/e0a", "0e", "0u/0ovi", "0em", "0y/0ové", "0ů", "0ům", "0y", "0y/0ové", "0ech", "0y"}},
        // otec
        {Gender::M, "-ec", {"ce", "ci/covi", "ec/ce", "če", "ci/covi", "cem", "ce/cové", "ců", "cům", "ce", "ce/cové", "cích", "ci"}},
        // učeň
        {Gender::M, "-([cčďšňřťž])", {"0e", "0i/0ovi", "0e", "0i", "0i/0ovi", "0em", "0e/0ové", "0ů", "0ům", "0e", "0e/0ové", "0ích", "0i"}},
        // boj
        {Gender::M, "-oj", {"oje", "oji/ojovi", "oj/oje", "oji", "oji/ojovi", "ojem", "oje/ojové", "ojů", "ojům", "oje", "oje/ojové", "ojích", "oji"}},
        // Bláha
        {Gender::M, "-([gh])a", {"0y", "0ovi", "0u", "0o", "0ovi", "0ou", "0ové", "0ů", "0ům", "0y", "0ové", "zích", "0y"}},
        // Rybka
        {Gender::M, "-([k])a", {"0y", "0ovi", "0u", "0o", "0ovi", "0ou", "0ové", "0ů", "0ům", "0y", "0ové", "cích", "0y"}},
        // Hála
        {Gender::M, "-a", {"y", "ovi", "u", "o", "ovi", "ou", "ové", "ů", "ům", "y", "ové", "ech", "y"}},
        // Avril
        {Gender::F, "-l", {"le", "li", "l", "li", "li", "lí", "le", "lí", "lím", "le", "le", "lích", "lemi"}},
        // ???
        {Gender::F, "-í", {"í", "í", "í", "í", "í", "í", "í", "ích", "ím", "í", "í", "ích", "ími"}},
        // beznaděj
        // TODO duplicate?
        {Gender::F, "-([jř])", {"0e", "0i", "0", "0i", "0i", "0í", "0e", "0í", "0ím", "0e", "0e", "0ích", "0emi"}},
        // Třebíč
        {Gender::F, "-([č])", {"0i", "0i", "0", "0i", "0i", "0í", "0i", "0í", "0ím", "0i", "0i", "0ích", "0mi"}},
        // Dobříš
        {Gender::F, "-([š])", {"0i", "0i", "0", "0i", "0i", "0í", "0i", "0í", "0ím", "0i", "0i", "0ích", "0emi"}},
        // Anatolije
        {Gender::S, "-([sljřň])e", {"0ete", "0eti", "0e", "0e", "0eti", "0etem", "0ata", "0at", "0atům", "0ata", "0ata", "0atech", "0aty"}},
        // čaj
        {Gender::M, "-j", {"je", "ji", "j", "ji", "ji", "jem", "je/jové", "jů", "jům", "je", "je/jové", "jích", "ji"}},
        // graf
        {Gender::M, "-f", {"fa", "fu/fovi", "f/fa", "fe", "fu/fovi", "fem", "fy/fové", "fů", "fům", "fy", "fy/fové", "fech", "fy"}},
        // Jiří
        {Gender::M, "-í", {"ího", "ímu", "ího", "í", "ímu", "ím", "í", "ích", "ím", "í", "í", "ích", "ími"}},
        // Hugo
        {Gender::M, "-go", {"a", "govi", "ga", "ga", "govi", "gem", "gové", "gů", "gům", "gy", "gové", "zích", "gy"}},
        // Kvido
        {Gender::M, "-o", {"a", "ovi", "a", "a", "ovi", "em", "ové", "ů", "ům", "y", "ové", "ech", "y"}},
        // šaty
        {Gender::ignore, "-([tp])y", {"", "", "", "", "", "", "0y", "0", "0ům", "0y", "0y", "0ech", "0ami"}},
        // dřeváky
        {Gender::ignore, "-([k])y", {"", "", "", "", "", "", "0y", "e0", "0ám", "0y", "0y", "0ách", "0ami"}},
        // ???
        {Gender::F, "-ar", {"ary", "aře", "ar", "ar", "ar", "ar", "ary", "ar", "arám", "ary", "ary", "arách", "arami"}},
        // madam
        {Gender::F, "-am", {"am", "am", "am", "am", "am", "am", "am", "am", "am", "am", "am", "am", "am"}},
        // Jennifer
        {Gender::F, "-er", {"er", "er", "er", "er", "er", "er", "ery", "er", "erám", "ery", "ery", "erách", "erami"}},
        // Joe
        {Gender::M, "-oe", {"oema", "oemovi", "oema", "oeme", "emovi", "emem", "oemové", "oemů", "oemům", "oemy", "oemové", "oemech", "oemy"}},

    };

    const std::string forceM[81] = {
        "alexej",
        "aleš",
        "alois",
        "ambrož",
        "ametyst",
        "andrej",
        "arest",
        "azbest",
        "bartoloměj",
        "bruno",
        "chlast",
        "chřest",
        "čaj",
        "čaroďej",
        "darmoďej",
        "dešť",
        "displej",
        "dobroďej",
        "drákula",
        "ďeda",
        "ďej",
        "host",
        "hranostaj",
        "hugo",
        "háj",
        "ilja",
        "ivo",
        "jirka",
        "jiří",
        "kolega",
        "koloďej",
        "komoří",
        "kontest",
        "koň",
        "kvido",
        "kříž",
        "lanýž",
        "lukáš",
        "maťej",
        "mikoláš",
        "mikuláš",
        "mluvka",
        "most",
        "motorest",
        "moula",
        "muž",
        "noe",
        "ondřej",
        "oto",
        "otto",
        "papež",
        "pepa",
        "peň",
        "plast",
        "podkoní",
        "polda",
        "prodej",
        "protest",
        "rest",
        "saša",
        "sleď",
        "slouha",
        "sluha",
        "sprej",
        "strejda",
        "stupeň",
        "sťežeň",
        "šmoula",
        "termoplast",
        "test",
        "tobiáš",
        "tomáš",
        "trest",
        "táta",
        "velmož",
        "výdej",
        "výprodej",
        "vězeň",
        "zeť",
        "zloďej",
        "žokej",
    };

    const std::string forceF[31] = {
        "dagmar",
        "dešť",
        "digestoř",
        "ester",
        "kancelář",
        "kleč",
        "konzervatoř",
        "koudel",
        "koupel",
        "křeč",
        "maštal",
        "miriam",
        "neteř",
        "obec",
        "ocel",
        "oratoř",
        "otep",
        "postel",
        "prdel",
        "rozkoš",
        "řeč",
        "sbeř",
        "trofej",
        "ves",
        "výstroj",
        "výzbroj",
        "vš",
        "zbroj",
        "zteč",
        "zvěř",
        "závěj",
    };

    const std::string forceS[11] = {
        "house",
        "kuře",
        "kůzle",
        "nemluvňe",
        "osle",
        "prase",
        "sele",
        "slůně",
        "tele",
        "vejce",
        "zvíře",
    };

    const Exception exceptions[44] = {
        {"bořek", "bořk", "bořka"},
        {"bořek", "bořk", "bořka"},
        {"chleba", "chleb", "chleba"},
        {"chléb", "chleb", "chleba"},
        {"cyklus", "cykl", "cyklus"},
        {"dvůr", "dvor", "dvůr"},
        {"déšť", "dešť", "déšť"},
        {"důl", "dol", "důl"},
        {"havel", "havl", "havla"},
        {"hnůj", "hnoj", "hnůj"},
        {"hůl", "hole", "hůl"},
        {"karel", "karl", "karla"},
        {"kel", "kl", "kel"},
        {"kotel", "kotl", "kotel"},
        {"kůň", "koň", "koně"},
        {"luděk", "luďk", "luďka"},
        {"líh", "lih", "líh"},
        {"mráz", "mraz", "mráz"},
        {"myš", "myše", "myš"},
        {"nehet", "neht", "nehet"},
        {"ocet", "oct", "octa"},
        {"osel", "osl", "osla"},
        {"pavel", "pavl", "pavla"},
        {"pes", "ps", "psa"},
        {"peň", "pň", "peň"},
        {"posel", "posl", "posla"},
        {"prsten", "prstýnek", "prstýnku"},
        {"pytel", "pytl", "pytel"},
        {"půl", "půle", "půli"},
        {"skrýš", "skrýše", "skrýš"},
        {"smrt", "smrť", "smrt"},
        {"sníh", "sněh", "sníh"},
        {"sopel", "sopl", "sopel"},
        {"stupeň", "stupň", "stupeň"},
        {"stůl", "stol", "stůl"},
        {"svatozář", "svatozáře", "svatozář"},
        {"sůl", "sole", "sůl"},
        {"tůň", "tůňe", "tůň"},
        {"účet", "účt", "účet"},
        {"veš", "vš", "veš"},
        {"vítr", "větr", "vítr"},
        {"vůl", "vol", "vola"},
        {"zeď", "zď", "zeď"},
        {"zář", "záře", "zář"},
    };

    std::vector<RegexPattern> regexes;
    std::vector<Pattern> dictionary;

public:
    Inflection()
    {
        pcrecpp::RE_Options opt(PCRE_UTF8 | PCRE_CASELESS);
        for (auto &pattern : patterns)
        {
            if ('-' == pattern.regex.at(0))
            {
                std::string tmp = "(";
                tmp.append(pattern.regex.substr(1)).append(")$");
                pcrecpp::RE regex(tmp, opt);
                RegexPattern rp = {regex, pattern};
                regexes.push_back(rp);
            }
            else
            {
                dictionary.push_back(pattern);
            }
        }
    }

    virtual ~Inflection() {}

    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }

    /**
     * @param string    $phrase
     * @param NULL|bool $animate
     */
    Php::Value inflect(Php::Parameters &params)
    {
        std::vector<std::string> words = split(params[0], ' ');
        std::reverse(words.begin(), words.end());

        std::vector<std::vector<std::string>> inflected;
        bool animate = false;
        if (params.size() >= 2)
        {
            animate = params[1]; // TODO cast properly
        }

        Gender gender = Gender::ignore;

        for (auto &word : words)
        {
            Php::out << "Matching word: " << word << "\n";
            std::string full;
            std::string glyph[3];
            bool useException = false;
            Exception e;

            if (gender == Gender::ignore)
            {
                for (auto &entry : forceM)
                {
                    if (word.compare(entry) == 0)
                    {
                        gender = Gender::M;
                        goto match;
                    }
                }
                for (auto &entry : forceF)
                {
                    if (word.compare(entry) == 0)
                    {
                        gender = Gender::F;
                        goto match;
                    }
                }
                for (auto &entry : forceS)
                {
                    if (word.compare(entry) == 0)
                    {
                        gender = Gender::S;
                        goto match;
                    }
                }
            }

            match:
            for (auto &entry : dictionary)
            {
                if ((gender == Gender::ignore || entry.gender == gender) && word.compare(entry.regex) == 0)
                {
                    std::vector<std::string> inflectedWord {word};
                    inflectedWord.insert(inflectedWord.end(), entry.cases, entry.cases + sizeof(entry.cases) / sizeof(entry.cases[0]));
                    inflected.push_back(inflectedWord);
                    Php::out << "matched word on dictionary: " << word << " -> " << entry.cases[5] << "\n";

                    if (gender == Gender::ignore)
                    {
                        gender = entry.gender;
                    }
                    goto nextWord;
                };
            }

            for (auto &exception : exceptions)
            {
                if (word.compare(exception.nominative) == 0)
                {
                    useException = true;
                    e = exception;
                    word = exception.stem;
                    break;
                }
            }
            for (auto &entry : regexes)
            {
                if (gender != Gender::ignore && entry.patern.gender != gender)
                {
                    continue;
                }

                bool found;
                const int glyphs = entry.regex.NumberOfCapturingGroups();
                switch (glyphs)
                {
                    default:
                    case 1:
                        found = entry.regex.PartialMatch(word, &full);
                        break;
                    case 2:
                        found = entry.regex.PartialMatch(word, &full, &glyph[0]);
                        break;
                    case 3:
                        found = entry.regex.PartialMatch(word, &full, &glyph[0], &glyph[1]);
                        break;
                    case 4:
                        found = entry.regex.PartialMatch(word, &full, &glyph[0], &glyph[1], &glyph[2]);
                        break;
                }
                if (found)
                {
                    Php::out << "matched word on regex: " << word << " -> " << entry.patern.cases[5] << "\n";

                    std::vector<std::string> inflectedWord;
                    inflectedWord.push_back(word);

                    auto prefix = word.substr(0, word.length() - full.length());
                    int ncase = 2;
                    for (auto &pcase : entry.patern.cases)
                    {
                        if (ncase == 4 && useException)
                        {
                            inflectedWord.push_back(e.accusative);
                            ncase++;
                            continue;
                        }

                        std::string partial = prefix;
                        auto posSlash = pcase.find('/');
                        if (posSlash != std::string::npos)
                        {
                            if (animate)
                            {
                                pcase = pcase.substr(posSlash);
                            }
                            else
                            {
                                pcase = pcase.substr(0, posSlash);
                            }
                        }

                        for (int i = 0; i < glyphs; i++)
                        {
                            auto groupPos = pcase.find((char) (i + 48));
                            if (groupPos == std::string::npos)
                            {
                                break;
                            }
                            pcase.replace(groupPos, 1, glyph[i]);
                        }

                        inflectedWord.push_back(partial.append(pcase));
                        ncase++;
                    }
                    inflected.push_back(inflectedWord);

                    if (gender == Gender::ignore)
                    {
                        gender = entry.patern.gender;
                    }
                    goto nextWord;
                }
            }

            nextWord:
            Php::out.flush();
        }

        Php::Value result;
        std::reverse(inflected.begin(), inflected.end());
        for (uint ncase = 1; ncase < 15; ncase++)
        {
            std::string inflectedPhrase;
            bool separate = false;
            for (auto &wordCases : inflected)
            {
                if (separate)
                {
                    inflectedPhrase.append(" ");
                }
                inflectedPhrase.append(wordCases.at(ncase - 1));
                separate = true;
            }

            result[ncase] = inflectedPhrase;
        }

        return result;


//        std::string phrase = params[0];
//
//        Php::out << "matching on '" << phrase << ">>>" << phrase.substr(1, 2) << "'\n";
//
//        pcrecpp::RE_Options opt(PCRE_UTF8);
//        pcrecpp::RE re("(.)ka$", opt);
//        if (re.error().length() > 0) {
//            Php::out << "PCRE compilation failed with error: " << re.error() << "\n";
//        }
//
//        if (re.PartialMatch(phrase, &glyph)) {
//            Php::out << "re match: " << glyph << "\n";
//        } else {
//            Php::out << "\033[31;mre not matched\033[m\n";
//        }
//
//        Php::out.flush();
//        return ++_value;
    }
    
};

extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  structure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("inflection", "1.0");
        
        Php::Class<Inflection> inflection("Inflection");
        inflection.method("inflect", &Inflection::inflect, { 
            Php::ByVal("phrase", Php::Type::String, true),
            Php::ByVal("animate", Php::Type::Bool, false)
        });

        extension.add(std::move(inflection));
        
        return extension;
    }
}
