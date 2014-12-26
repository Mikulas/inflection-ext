#include <phpcpp.h>

/**
 *  Inflection class that can be used for counting
 */
class Inflection : public Php::Base
{
private:
    /**
     *  The initial value
     *  @var    int
     */
    int _value = 0;

public:
    /**
     *  C++ constructor and destructor
     */
    Inflection() {}
    virtual ~Inflection() {}
    
    Php::Value inflect(Php::Parameters &params)
    {
        return ++_value;
    }
    
};

/**
 *  tell the compiler that the get_module is a pure C function
 */
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
        
        // description of the class so that PHP knows which methods are accessible
        Php::Class<Inflection> inflection("Inflection");
        inflection.method("inflect", &Inflection::inflect, { 
            Php::ByVal("phrase", Php::Type::String, true),
            Php::ByVal("animate", Php::Type::Bool, false)
        });

        // add the class to the extension
        extension.add(std::move(inflection));
        
        // return the extension
        return extension;
    }
}
