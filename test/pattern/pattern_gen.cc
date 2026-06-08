#include "CLI/App.hpp"
#include "CLI/Error.hpp"
#include "trieste/trieste.h"

#include <CLI/CLI.hpp>
#include <cstddef>
#include <random>


using namespace trieste;

static void comma_separate_tokens(Node pattern, std::stringstream& ss)
{
  bool first = true;
  for (auto& token_node : *pattern)
  {
    if (!first)
      ss << ", ";
    Location loc = token_node->location();
    ss << loc.view();
    first = false;
  }
}

// Convert a pattern to a string for error messages.
static std::string pattern_to_string(Node pattern)
{
  if (pattern == Top)
    pattern = pattern / Group;

  std::stringstream ss;
  if (pattern == reified::First)
  {
    ss << "Start";
  }
  else if (pattern == reified::Last)
  {
    ss << "End";
  }
  else if (pattern == reified::Any)
  {
    ss << "Any";
  }
  else if (pattern == reified::TokenMatch)
  {
    ss << "T(";
    comma_separate_tokens(pattern, ss);
    ss << ")";
  }
  else if (pattern == reified::RegexMatch)
  {
    auto token_node = pattern / reified::Token;
    Location loc = token_node->location();
    std::string regex =
      std::string((pattern / reified::Regex)->location().view());
    ss << "T(" << loc.view() << ", \"" << regex << "\")";
  }
  else if (pattern == reified::Cap)
  {
    std::string name =
      std::string((pattern / reified::Token)->location().view());
    ss << "(" << pattern_to_string(pattern / Group) << ")[" << name << "]";
  }
  else if (pattern == reified::Opt)
  {
    ss << "~(" << pattern_to_string(pattern / Group) << ")";
  }
  else if (pattern == reified::Rep)
  {
    ss << "(" << pattern_to_string(pattern / Group) << ")++";
  }
  else if (pattern == reified::Not)
  {
    ss << "!(" << pattern_to_string(pattern / Group) << ")";
  }
  else if (pattern == reified::Choice)
  {
    ss << "(" << pattern_to_string(pattern / reified::First) << ") / ("
       << pattern_to_string(pattern / reified::Last) << ")";
  }
  else if (pattern == reified::InsideStar)
  {
    ss << "In(";
    comma_separate_tokens(pattern, ss);
    ss << ")++";
  }
  else if (pattern == reified::Inside)
  {
    ss << "In(";
    comma_separate_tokens(pattern, ss);
    ss << ")";
  }
  else if (pattern == reified::Children)
  {
    ss << "(" << pattern_to_string(pattern / Group) << ") << ("
       << pattern_to_string(pattern / reified::Children) << ")";
  }
  else if (pattern == reified::Pred)
  {
    ss << "++(" << pattern_to_string(pattern / Group) << ")";
  }
  else if (pattern == reified::NegPred)
  {
    ss << "--(" << pattern_to_string(pattern / Group) << ")";
  }
  else if (pattern == reified::Action)
  {
    ss << "((" << pattern_to_string(pattern / Group)
       << ")([](auto) { return true; }))";
  }
  else // Group
  {
    bool first = true;
    for (auto& child : *pattern)
    {
      if (!first)
        ss << " * ";
      ss << pattern_to_string(child);
      first = false;
    }
  }
  return ss.str();
}

int main(int argc, char** argv)
{
  auto app = CLI::App("Pattern generator for Trieste tests");

  std::size_t target_depth = 10;
  app.add_option("-d", target_depth, "Target depth");

  uint32_t seed = std::random_device()();
  app.add_option("-s,--seed", seed, "Random seed");

  try
  {
    app.parse(argc, argv);
  }
  catch (CLI::ParseError& err)
  {
    return app.exit(err);
  }

  auto wf = reified::pattern_wf;
  GenNodeLocationF gen = [](Rand& rand, Node node) {
    if (node == reified::Token)
    {
      auto r = rand() % 5;
      return Location("Tok" + std::to_string(r));
    }
    else if (node == reified::Regex)
    {
      return Location("regex");
    }
    return Location("<generated>");
  };

//   std::cout << "Generating pattern with seed " << seed << " and target depth "
            // << target_depth << std::endl;
  Node pat = wf.gen(gen, seed, target_depth, false);
  WFContext context;
  context.push_back(wf);
  std::cout << pattern_to_string(pat) << std::endl;
  context.pop_front();
  return 0;
}