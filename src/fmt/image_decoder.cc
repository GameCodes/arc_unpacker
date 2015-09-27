#include "fmt/image_decoder.h"
#include "err.h"
#include "fmt/naming_strategies.h"
#include "util/image.h"

using namespace au;
using namespace au::fmt;

ImageDecoder::~ImageDecoder()
{
}

std::unique_ptr<INamingStrategy> ImageDecoder::naming_strategy() const
{
    return std::unique_ptr<INamingStrategy>(new SiblingNamingStrategy);
}

void ImageDecoder::register_cli_options(ArgParser &) const
{
}

void ImageDecoder::parse_cli_options(const ArgParser &)
{
}

bool ImageDecoder::is_recognized(File &file) const
{
    try
    {
        file.io.seek(0);
        return is_recognized_internal(file);
    }
    catch (...)
    {
        return false;
    }
}

void ImageDecoder::unpack(
    File &input_file, FileSaver &file_saver, bool recurse) const
{
    auto output_grid = decode(input_file);
    auto output_file = util::grid_to_boxed(output_grid, input_file.name);
    output_file->name
        = boost::filesystem::path(output_file->name).filename().string();
    file_saver.save(std::move(output_file));
}

pix::Grid ImageDecoder::decode(File &file) const
{
    if (!is_recognized(file))
        throw err::RecognitionError();

    file.io.seek(0);
    return decode_internal(file);
}
