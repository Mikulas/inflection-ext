#include <phpcpp.h>
#include <iostream>
#include <string>
#include <sstream>
#include <pcrecpp.h>

enum class Gender { M, F, N }; // masculine, feminine, neuter

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

class Inflection : public Php::Base
{
private:
    const Pattern patterns[2] = {
        {Gender::M, "-ná", {"kého", "", "", "ký", "kém", "kým", "ké/cí", "kých", "kým", "ké", "ké/cí", "kých", "kými"}},
        {Gender::M, "on", {"něho", "", "", "on", "něm", "ním", "oni", "nich", "nim", "je", "oni", "nich", "jimi/nimi"}},
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

        for (auto &word : words)
        {
            Php::out << "Matching word: " << word << "\n";
            std::string full, glyph1, glyph2, glyph3;

            for (auto &entry : dictionary)
            {
                if (word.compare(entry.regex) == 0)
                {
                    std::vector<std::string> inflectedWord {word};
                    inflectedWord.insert(inflectedWord.end(), entry.cases, entry.cases + sizeof(entry.cases) / sizeof(entry.cases[0]));
                    inflected.push_back(inflectedWord);
                    Php::out << "matched word on dictionary: " << word << " -> " << entry.cases[5] << "\n";
                    goto nextWord;
                };
            }
            for (auto &entry : regexes)
            {
                bool found;
                switch (entry.regex.NumberOfCapturingGroups())
                {
                    default:
                    case 1:
                        found = entry.regex.PartialMatch(word, &full);
                        break;
                    case 2:
                        found = entry.regex.PartialMatch(word, &full, &glyph1);
                        break;
                    case 3:
                        found = entry.regex.PartialMatch(word, &full, &glyph1, &glyph2);
                        break;
                    case 4:
                        found = entry.regex.PartialMatch(word, &full, &glyph1, &glyph2, &glyph3);
                        break;
                }
                if (found)
                {
                    Php::out << "matched word on regex: " << word << " -> " << entry.patern.cases[5] << "\n";

                    std::vector<std::string> inflectedWord;
                    inflectedWord.push_back(word);

                    auto prefix = word.substr(0, word.length() - full.length());
                    for (auto &pcase : entry.patern.cases)
                    {
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
                        inflectedWord.push_back(partial.append(pcase));
                    }
                    inflected.push_back(inflectedWord);
                    goto nextWord;
                }
            }

            nextWord:
            Php::out.flush();
        }

        Php::Value result;
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
