#ifndef _DUMB_CSV_PARSER_H_
#define _DUMB_CSV_PARSER_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <stdexcept>


/** EXAMPLE OF USE **
 *
 *  dcp::dumb_csv_parser in(dcp::options::read_header | dcp::options:force_header_match);
 *  in.config(",", "\n");
 *  in.parse("filename.csv");
 *
 *  dcp::csv_row header;
 *  if(in.fetch_header(header)
 *  {
 *      dcp::csv_row row;
 *      while(in.fetch_row(row))
 *      {
 *          for(dcp::csv_row::iterator it = row.begin(); it != row.end(); ++it)
 *              if(it == row.begin()) {std::cout << *it;} else {std::cout << ", " << *it; }
 *          std::cout << std::endl;
 *      }
 *  }
 *
 */

namespace dcp {
    // ANSI/UNICODE definitions
    typedef std::string             string;
    typedef std::ostringstream      ostringstream;

    typedef std::vector<std::vector<dcp::string> > csv_table;
    typedef std::vector<dcp::string> csv_row;
    typedef dcp::string csv_field;

    typedef std::vector<char> fbytes;
    typedef unsigned char byte;

    dcp::string str_replace(const dcp::string &, const dcp::string &, const dcp::string &);

    enum class options {
        no_opts             = 0x00,
        no_header           = 0x01,
        skip_header         = 0x02,
        read_header         = 0x04,
        ignore_mismatch     = 0x08,
        force_header_match  = 0x10
    };

    inline dcp::options operator|(dcp::options a, dcp::options b) { return (dcp::options)((byte)a | (byte)b); }

    class dumb_csv_parser
    {

    public:
        /*  Brief:  Defualt Constructor
         *
         *  @return class instance
         */
        dumb_csv_parser(void);
        /*  Brief:  Constructor with options on how to read the CSV;
         *          options can be found as dcp::options bit flags.
         *
         *  @opts   dcp::options comma separated as needed
         *
         *  @return class instance
         */
        dumb_csv_parser(dcp::options opts);
        /* Brief:   Default destructor, auto cleanup on all data
         */
        ~dumb_csv_parser(void);

        /*  Brief:  Configs row and field separator constants;
         *
         *  @fs     Field separator value; default = ","
         *  @rs     Row separator value; default = OS specified
         */
        void    config(const dcp::string fs, const dcp::string rs);

        /*  Brief:  Parses a CSV file by first reading it into memory
         *          and then reading field-by-field and allocating
         *          each row to the class table structure. As fields
         *          are read they are eliminated from the file memory
         *          thus transferring file memory to class memory
         *          as each field is consumed to keep memoery relatively
         *          constant and ensuring there is at most enough memory
         *          to store the CSV contents.
         *
         *  @csv   CSV filepath, either relative or full resolved
         */
        void    parse(const dcp::string csv);

        /*  Brief:  Will fetch the header row if dcp::options::read_header
         *          was set in the initialisation.
         *
         *  @row    Vector reference to fill with the header row
         *
         *  @return TRUE/FALSE to indicate if a header row was fetched
         */
        bool    fetch_header(csv_row & row);
        /*  Brief:  Will fetch row-by-row until all rows are consumed
         *          starting from the 1st non-header row - if the
         *          dcp::option::read_header was declared this is
         *          row 2 in the table.
         *
         *  @row    Vector reference to fill with the current row
         *
         *  @return TRUE/FALSE to indicate if a row was fetched
         */
        bool    fetch_row(csv_row & row);

        /*  Brief:  Clears the contents of the table vector.
         */
        void    clear(void);

        /*  Brief:  Prints the table vector.
         */
        void    print_table(void);

    private:
        void                add_field(dcp::csv_row&, dcp::fbytes::iterator&, dcp::fbytes::iterator&);
        void                add_row(dcp::csv_row&);
        void                rummage(dcp::fbytes::iterator *, int);
        static dcp::fbytes  read_all_bytes(const dcp::string);
        bool                is_row_separator(dcp::fbytes::iterator);
        bool                is_field_separator(dcp::fbytes::iterator);

        dcp::csv_table              table;
        dcp::csv_table::iterator    row_ptr;
        dcp::byte                   opts;
        dcp::string                 fs;
        dcp::string                 rs;
    };

}
#endif
