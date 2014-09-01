Dumb CSV Parser
===============

Dumb CSV Parser : Quick and simple CSV Parser

The idea around the Dumb CSV Parser (DCP) is to be <b>dumb</b> !

The parser ignores all field information and has a pure goal of finding the end marker per field.

It follows the <a href="http://en.wikipedia.org/wiki/Comma-separated_values">RFC 4180</a> standard where '"' is the escape character for quotes '"' (i.e. double quote escape).

Quotes are optional to encapsulate a field that contains a row or field separator (i.e. default field separator = ',' and default row separator = '\n').

Several options can be set for the parser, these must be done in the constructor using '|' operator fpr every option required (N.B. some options make others obsolete) <pre>dcp::dumb_csv_parser(dcp::options::skip_header | dcp::options::ignore_mismatch)</pre>

Custom field/row separators can be configured using <pre>dcp::dumb_csv_parser::config</pre>.

Example Code:

<syntaxhighlight lang="cpp" line start="1">

    #include "dumb_csv_parser.hpp"

    int main(int argc, char ** argv)
    {
        dcp::dumb_csv_parser in(dcp::options::read_header |
                                dcp::options::ignore_mismatch);
        
        in.parse(argv[0]);
        
        dcp::csv_row header;
        int line_counter = 1;
        
        if(in.fetch_header(header))
        {
            dcp::csv_row row;
               
            while(in.fetch_row(row))
            {
                if(header.size() == row.size())
                {
                    std::cout << "Row " << line_counter << std::endl;
                    for(int i = 0; i < header.size(); ++i)
                        std::cout << "    " << header[i] << " --> " << row[i] << std::endl;
                }
                else
                    std::cout << "Row " << line_counter << " - Error: header-row mismatch" << std::endl;
                    
                ++line_counter;
            }
        }
        
        return 0;
    }
    
</syntaxhighlight>
