#include "fmt/french_bread/p_archive_decoder.h"
#include "algo/range.h"
#include "err.h"

using namespace au;
using namespace au::fmt::french_bread;

namespace
{
    struct ArchiveEntryImpl final : fmt::ArchiveEntry
    {
        size_t offset;
        size_t size;
    };
}

bool PArchiveDecoder::is_recognized_impl(io::File &input_file) const
{
    auto meta = read_meta(input_file);
    if (!meta->entries.size())
        return false;
    auto last_entry = static_cast<ArchiveEntryImpl*>(
        meta->entries[meta->entries.size() - 1].get());
    return last_entry->offset + last_entry->size == input_file.stream.size();
}

std::unique_ptr<fmt::ArchiveMeta>
    PArchiveDecoder::read_meta_impl(io::File &input_file) const
{
    static const u32 encryption_key = 0xE3DF59AC;
    input_file.stream.seek(0);
    auto magic = input_file.stream.read_u32_le();
    auto file_count = input_file.stream.read_u32_le() ^ encryption_key;
    if (magic != 0 && magic != 1)
        throw err::RecognitionError();
    auto meta = std::make_unique<ArchiveMeta>();
    for (auto i : algo::range(file_count))
    {
        auto entry = std::make_unique<ArchiveEntryImpl>();
        auto name = input_file.stream.read(60).str();
        for (auto j : algo::range(name.size()))
            name[j] ^= i * j * 3 + 0x3D;
        entry->path = name.substr(0, name.find('\0'));
        entry->offset = input_file.stream.read_u32_le();
        entry->size = input_file.stream.read_u32_le() ^ encryption_key;
        meta->entries.push_back(std::move(entry));
    }
    return meta;
}

std::unique_ptr<io::File> PArchiveDecoder::read_file_impl(
    io::File &input_file,
    const fmt::ArchiveMeta &m,
    const fmt::ArchiveEntry &e) const
{
    const auto entry = static_cast<const ArchiveEntryImpl*>(&e);
    const auto key = entry->path.str();
    auto data = input_file.stream.seek(entry->offset).read(entry->size);
    static const size_t encrypted_block_size = 0x2173;
    for (auto i : algo::range(std::min(encrypted_block_size, entry->size)))
        data[i] ^= key[i % key.size()] + i + 3;
    return std::make_unique<io::File>(entry->path, data);
}

std::vector<std::string> PArchiveDecoder::get_linked_formats() const
{
    return {"french-bread/ex3"};
}

static auto dummy = fmt::register_fmt<PArchiveDecoder>("french-bread/p");
