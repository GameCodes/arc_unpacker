#include "fmt/leaf/cz10_image_archive_decoder.h"
#include "test_support/catch.hh"
#include "test_support/decoder_support.h"
#include "test_support/file_support.h"
#include "test_support/image_support.h"

using namespace au;
using namespace au::fmt::leaf;

static const std::string dir = "tests/fmt/leaf/files/cz10/";

static void do_test(
    const std::string &input_path,
    const std::vector<std::string> &expected_paths)
{
    const Cz10ImageArchiveDecoder decoder;
    const auto input_file = tests::file_from_path(dir + input_path);
    const auto actual_files = tests::unpack(decoder, *input_file);
    std::vector<std::shared_ptr<pix::Grid>> expected_images;
    for (const auto &path : expected_paths)
        expected_images.push_back(tests::image_from_path(dir + path));
    tests::compare_images(expected_images, actual_files);
}

TEST_CASE("Leaf CZ10 images", "[fmt]")
{
    SECTION("Encoding type 1+2+3, regular channel order")
    {
        do_test("cbg01_0.cz1", {"cbg01_0-out.png"});
    }

    SECTION("Encoding type 0, multi images")
    {
        do_test(
            "chi06_2.cz1",
            {
                "chi06_2_000-out.png",
                "chi06_2_001-out.png",
                "chi06_2_002-out.png",
                "chi06_2_003-out.png",
                "chi06_2_004-out.png",
            });
    }
}
