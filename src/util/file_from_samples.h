#pragma once

#include <memory>
#include "file.h"

namespace au {
namespace util {

    std::unique_ptr<File> file_from_samples(
        size_t channel_count,
        size_t bytes_per_sample,
        size_t sample_rate,
        const bstr &samples,
        const std::string &name);

} }