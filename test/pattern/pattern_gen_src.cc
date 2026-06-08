// Skeleton file for running performance tests on generated patterns

#include "trieste/trieste.h"
#include <chrono>

using namespace trieste;


inline const auto Tok0 = trieste::TokenDef("tok0");
inline const auto Tok1 = trieste::TokenDef("tok1");
inline const auto Tok2 = trieste::TokenDef("tok2");
inline const auto Tok3 = trieste::TokenDef("tok3");
inline const auto Tok4 = trieste::TokenDef("tok4");

Node effect(Match&) { return Top; }

const size_t counts = /*counts*/;
const size_t depths = /*depths*/;

/*patterns*/

static std::function<std::vector<detail::PatternTreeEffect<Node>>(void)> functions[counts][depths];

void populate_functions() 
{
/*functions*/
}

using sclock = std::chrono::steady_clock;

int main(int argc, char* argv[])
{
    populate_functions();

    int count_index = 0;
    int depth_index = 0;

    if(argc > 1)
        count_index = atoi(argv[1]) - 1;
    
    if(argc > 2)
        depth_index = atoi(argv[2]) - 1;

    std::vector<detail::PatternEffect<Node>> results;

    sclock::time_point parse_start = sclock::now();

    std::vector<detail::PatternTreeEffect<Node>> pattern_fxs = functions[count_index][depth_index]();

    // Remove if running on original Trieste
    for(auto& pfx : pattern_fxs)
    {
        results.push_back(pfx.compile());
    }

    sclock::time_point compile_end = sclock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(compile_end - parse_start) << std::endl;

    return 0;
}