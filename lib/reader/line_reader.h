#pragma once

#include <iostream>
#include <memory>
#include <sstream>

namespace PaceVC {

/**
 * Ignores comments
 */
class LineReader {
public:
    LineReader(std::istream& is);
    ~LineReader();

    std::istringstream nextLine();

private:
    class TImpl;
    std::unique_ptr<TImpl> Impl_;
};

}
