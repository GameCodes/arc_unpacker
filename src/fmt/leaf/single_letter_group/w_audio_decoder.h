#pragma once

#include "fmt/audio_decoder.h"

namespace au {
namespace fmt {
namespace leaf {

    class WAudioDecoder final : public AudioDecoder
    {
    protected:
        bool is_recognized_impl(io::File &input_file) const override;
        sfx::Audio decode_impl(io::File &input_file) const override;
    };

} } }