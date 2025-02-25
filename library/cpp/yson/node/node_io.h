#pragma once

#include "node.h"
#include <library/cpp/yson/public.h>

namespace NJson {
    class TJsonValue;
} // namespace NJson

namespace NYT {

////////////////////////////////////////////////////////////////////////////////

// Parse TNode from string in YSON format
TNode NodeFromYsonString(const TStringBuf input, ::NYson::EYsonType type = ::NYson::EYsonType::Node);

// Serialize TNode to string in one of YSON formats with random order of maps' keys (don't use in tests)
TString NodeToYsonString(const TNode& node, ::NYson::EYsonFormat format = ::NYson::EYsonFormat::Text);

// Same as the latter, but maps' keys are sorted lexicographically (to be used in tests)
TString NodeToCanonicalYsonString(const TNode& node, ::NYson::EYsonFormat format = ::NYson::EYsonFormat::Text);

// Parse TNode from stream in YSON format
TNode NodeFromYsonStream(IInputStream* input, ::NYson::EYsonType type = ::NYson::EYsonType::Node);

// Serialize TNode to stream in one of YSON formats with random order of maps' keys (don't use in tests)
void NodeToYsonStream(const TNode& node, IOutputStream* output, ::NYson::EYsonFormat format = ::NYson::EYsonFormat::Text);

// Same as the latter, but maps' keys are sorted lexicographically (to be used in tests)
void NodeToCanonicalYsonStream(const TNode& node, IOutputStream* output, ::NYson::EYsonFormat format = ::NYson::EYsonFormat::Text);

// Parse TNode from string in JSON format
TNode NodeFromJsonString(const TStringBuf input);

// Convert TJsonValue to TNode
TNode NodeFromJsonValue(const NJson::TJsonValue& input);

////////////////////////////////////////////////////////////////////////////////

} // namespace NYT
