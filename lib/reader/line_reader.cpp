#include <reader/line_reader.h>

#include <string>

namespace PaceVC {

class LineReader::TImpl {
public:
    TImpl(std::istream& is) : is_(is) {}

    std::istringstream nextLine() {
        return std::istringstream(skip());
    }

private:
    std::string skip() {
        std::string line = readLine();
        while (!line.empty() && line[0] == 'c')
            line = readLine();
        return line;
    }

    std::string readLine() {
        std::string ret;
        std::getline(is_, ret);
        return ret;
    }

    std::istream& is_;
};

LineReader::LineReader(std::istream& is)
    : Impl_(std::make_unique<LineReader::TImpl>(is)) {}

LineReader::~LineReader() = default;

std::istringstream LineReader::nextLine() {
    return Impl_->nextLine();
}

}
