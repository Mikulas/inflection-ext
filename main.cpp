#include <phpcpp.h>
#include <iostream>
#include <string>
#include <pcrecpp.h>

class Inflection : public Php::Base
{
private:
    /**
     *  The initial value
     *  @var    int
     */
    int _value = 0;

public:
    Inflection() {}
    virtual ~Inflection() {}
    
    /**
     * @param string    $phrase
     * @param NULL|bool $animate
     */
    Php::Value inflect(Php::Parameters &params)
    {
        string phrase = params[0];
        string glyph;
       
        Php::out << "matching on '" << phrase << "'\n";

        pcrecpp::RE_Options opt(PCRE_UTF8);
        pcrecpp::RE re(">(.)<", opt);
        if (re.error().length() > 0) {
            Php::out << "PCRE compilation failed with error: " << re.error() << "\n";
        }

        if (re.PartialMatch(phrase, &glyph)) {
            Php::out << "re match: " << glyph << "\n";
        } else {
            Php::out << "\033[31;mre not matched\033[m\n";
        }

        Php::out.flush();
        return ++_value;
    }
    
};

extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
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
