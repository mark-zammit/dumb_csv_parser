#include "dumb_csv_parser.hpp"

#define END_QUOTED(x)           ((x == 2 || x == 0))
#define ADJ_QUOTE_COUNT(x,y)    (x == '"' ? y == 2 ? 1 : y + 1 : y)

using namespace dcp;

dcp::string dcp::str_replace(const dcp::string & str, const dcp::string & search, const dcp::string & replace)
{
    dcp::string ret = str;
    size_t pos = 0;
    while((pos = ret.find(search, pos)) != dcp::string::npos)
    {
        ret.replace(pos, search.length(), replace);
        pos += replace.length();
    }

    return ret;
}

dumb_csv_parser::dumb_csv_parser(void) : opts(0), row_ptr(table.end())
{
    dcp::ostringstream nl;
    nl << std::endl;
    config(",", nl.str());
}
dumb_csv_parser::dumb_csv_parser(dcp::options opts) : opts((byte)opts)
{
    dcp::ostringstream nl;
    nl << std::endl;
    config(",", nl.str());
}
dumb_csv_parser::~dumb_csv_parser(void) {}

void dumb_csv_parser::config(const dcp::string fs, const dcp::string rs)
{
    this->fs = fs;
    this->rs = rs;
}

void dumb_csv_parser::parse(const dcp::string csv)
{
    static fbytes data = this->read_all_bytes(csv.c_str());
    fbytes::iterator start = data.begin();
    csv_row row;

    while(start != data.end())
    {
        fbytes::iterator ptr = start;
        int quotes = 0;

        rummage(&ptr, quotes);

        if(ptr+1 == data.end())
            break;

        add_field(row, start, ptr);

        if(is_row_separator((ptr+1)))           { this->add_row(row); data.erase(data.begin(), ptr+1+this->rs.length()); }
        else if(is_field_separator((ptr+1)))    { data.erase(data.begin(), ptr+1+this->fs.length()); }
        else                                    { data.erase(data.begin(), data.end()); }

        start = data.begin();
    }

    this->add_row(row);
}

bool dumb_csv_parser::fetch_header(csv_row & row)
{
    if((opts & (byte)options::read_header))
        row = table[0];
    else
        row.clear();

    return ((opts & (byte)options::read_header) ? true : false);
}

bool dumb_csv_parser::fetch_row(csv_row & row)
{
    if(row_ptr!=table.end())
    {
        row = (*row_ptr);
        ++row_ptr;
        return true;
    }
    else
    {
        row_ptr = table.begin();
        return false;
    }

    return false;
}



fbytes dumb_csv_parser::read_all_bytes(const dcp::string filename)
{
    struct stat buf;
    if(stat(filename.c_str(),&buf) == -1)
        throw std::invalid_argument(dcp::string("FileNotFoundException: Unable to locate/open file '") +
                                    filename +
                                    dcp::string("'."));

    std::ifstream ifs(filename.c_str(), std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    fbytes  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);
    result.push_back(EOF);

    return result;
}

void dumb_csv_parser::add_field(dcp::csv_row & row, dcp::fbytes::iterator & start, dcp::fbytes::iterator & end)
{
    if(*start == '"' && *end == '"')
        row.push_back(dcp::str_replace(csv_field(start+1,end), "\"\"", "\""));
    else
        row.push_back(dcp::str_replace(csv_field(start,end+1), "\"\"", "\""));
}

void dumb_csv_parser::add_row(dcp::csv_row & row)
{
    if(!row.empty())
        if(((opts & (byte)options::skip_header) && !table.empty()) || !(opts & (byte)options::skip_header))
        {
            if((opts & (byte)options::read_header) &&
               (opts & (byte)options::force_header_match) && !table.empty() && row.size() == table[0].size())
                table.push_back(row);
            else if(!(opts & (byte)options::read_header) || (opts & (byte)options::ignore_mismatch) || !opts || table.empty())
                table.push_back(row);
        }
    row.clear();
    row_ptr = table.begin()+((opts & (byte)options::read_header) > 0);
}

void dumb_csv_parser::rummage(fbytes::iterator * p, int q)
{
    if((**p == EOF || (END_QUOTED(q) && (is_field_separator(*p) || is_row_separator(*p)))) && (int)*(--*p))
        return;
    rummage(&(++*p), ADJ_QUOTE_COUNT(**p,q));
}

bool dumb_csv_parser::is_row_separator(dcp::fbytes::iterator p)
{
    try {
        return (this->rs.compare(dcp::string(p, p+rs.length())) == 0);
    } catch (...) {
        return false;
    }
}

bool dumb_csv_parser::is_field_separator(dcp::fbytes::iterator p)
{
    try {
        return (this->fs.compare(dcp::string(p, p+fs.length())) == 0);
    } catch (...) {
        return false;
    }
}

void dumb_csv_parser::clear(void)
{
    this->table.clear();
}

void dumb_csv_parser::print_table(void)
{
    for(csv_table::iterator it = table.begin(); it != table.end(); ++it)
    {
        for(csv_row::iterator itr = it->begin(); itr != it->end(); ++itr)
        {
            if(itr == it->begin())
                std::cout << *itr;
            else
                std::cout << ", " << *itr;
        }
        std::cout << std::endl;
    }
}
