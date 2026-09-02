#pragma once

#include <stdint.h>

#include <vector>

#include "Utils/FileStream.h"

namespace TitaniumDecompiler {
struct LineNumTableItem {
    LineNumTableItem() {}
    ~LineNumTableItem() = default;
    uint16_t startPc;
    uint16_t lineNumber;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            LineNumTableItem& instance);
};

struct LocalVariableTableEntry {
    LocalVariableTableEntry() {}
    ~LocalVariableTableEntry() = default;
    uint16_t startPc;
    uint16_t length;
    uint16_t nameIndex;
    uint16_t descIndex;
    uint16_t index;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            LocalVariableTableEntry& instance);
};

struct Classes {
    Classes() {}
    ~Classes() = default;
    uint16_t innerClassInfoIdx;
    uint16_t outerClassInfoIdx;
    uint16_t innerNameIdx;
    uint16_t innerClassAccessFlags;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            Classes& instance);
};

struct LocalVariableTypeTable {
    LocalVariableTypeTable() {}
    ~LocalVariableTypeTable() = default;
    uint16_t startPc;
    uint16_t length;
    uint16_t nameIdx;
    uint16_t sigIdx;
    uint16_t idx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            LocalVariableTypeTable& instance);
};

struct BootstrapMethodsInner {
    BootstrapMethodsInner() {}
    ~BootstrapMethodsInner() = default;

    uint16_t bootstrapMethodRef;
    uint16_t numBootstrapArgs;
    std::vector<uint16_t> bootstrapArgs;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            BootstrapMethodsInner& instance);
};

struct Parameters {
    Parameters() {}
    ~Parameters() = default;

    uint16_t nameIdx;
    uint16_t accessFlags;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            Parameters& instance);
};

struct Annotation;
// struct ElementValue;

struct EnumConstValue {
    EnumConstValue() {}
    ~EnumConstValue() = default;

    uint16_t typeNameIdx;
    uint16_t constNameIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            EnumConstValue& instance);
};

struct ArrayValue;
// MOVE ElementValue BEFORE ArrayValue!
struct ElementValue {
    ElementValue() {}
    ~ElementValue() = default;

    uint8_t tag;
    struct Value {
        Value() {}
        ~Value() = default;

        uint16_t constValIdx;
        EnumConstValue enumConstVal;
        uint16_t classInfoIdx;
        std::shared_ptr<Annotation> annotationVal;
        std::shared_ptr<ArrayValue>
            arrVal;  // <-- Use pointer instead of direct reference
        static void Deserialize(BigEndianStreamReader* deserializer,
                                Value& instance);
    } value;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            ElementValue& instance);
};

struct ArrayValue {
    ArrayValue() {}
    ~ArrayValue() = default;

    uint16_t numVals;
    std::vector<ElementValue> values;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            ArrayValue& instance);
};

struct ElementValuePairs {
    ElementValuePairs() {}
    ~ElementValuePairs() = default;

    uint16_t elemNameIdx;
    ElementValue value;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            ElementValuePairs& instance);
};

struct Annotation {
    Annotation() {}
    ~Annotation() = default;

    uint16_t typeIdx;
    uint16_t numElemValPairs;
    std::vector<ElementValuePairs> elemValPairs;
    static void Deserialize(BigEndianStreamReader* deserializer,
                            Annotation& instance);
};

struct ParameterAnnotations {
    ParameterAnnotations() {}
    ~ParameterAnnotations() = default;

    uint16_t numAnnotations;
    std::vector<Annotation> annotations;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            ParameterAnnotations& instance);
};

struct TypeParameterTarget {
    TypeParameterTarget() {}
    ~TypeParameterTarget() = default;

    uint8_t typeParamIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            TypeParameterTarget& instance);
};

struct SuperTypeTarget {
    SuperTypeTarget() {}
    ~SuperTypeTarget() = default;

    uint16_t superTypeIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            SuperTypeTarget& instance);
};

struct TypeParameterBoundTarget {
    TypeParameterBoundTarget() {}
    ~TypeParameterBoundTarget() = default;

    uint8_t typeParamIdx;
    uint8_t boundIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            TypeParameterBoundTarget& instance);
};

struct EmptyTarget {
    EmptyTarget() {}
    ~EmptyTarget() = default;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            EmptyTarget& instance);
};

struct FormalParameterTarget {
    FormalParameterTarget() {}
    ~FormalParameterTarget() = default;

    uint8_t formalParamIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            FormalParameterTarget& instance);
};

struct ThrowsTarget {
    ThrowsTarget() {}
    ~ThrowsTarget() = default;

    uint16_t throwsTypeIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            ThrowsTarget& instance);
};

struct LocalVarTargetTable {
    LocalVarTargetTable() {}
    ~LocalVarTargetTable() = default;

    uint16_t startPc;
    uint16_t length;
    uint16_t index;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            LocalVarTargetTable& instance);
};

struct LocalVarTarget {
    LocalVarTarget() {}
    ~LocalVarTarget() = default;

    uint16_t tableLength;
    std::vector<LocalVarTargetTable> table;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            LocalVarTarget& instance);
};

struct CatchTarget {
    CatchTarget() {}
    ~CatchTarget() = default;

    uint16_t exceptionTableIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            CatchTarget& instance);
};

struct OffsetTarget {
    OffsetTarget() {}
    ~OffsetTarget() = default;

    uint16_t offset;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            OffsetTarget& instance);
};

struct TypeArgumentTarget {
    TypeArgumentTarget() {}
    ~TypeArgumentTarget() = default;

    uint16_t offset;
    uint8_t typeArgIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            TypeArgumentTarget& instance);
};

struct TargetInfo {
    TargetInfo() {}
    ~TargetInfo() = default;

    TypeParameterTarget typeParamTarget;
    SuperTypeTarget superTypeTarget;
    TypeParameterBoundTarget typeParamBoundTarget;
    EmptyTarget emptyTarget;
    FormalParameterTarget formalParameterTarget;
    ThrowsTarget throwsTarget;
    LocalVarTarget localvarTarget;
    CatchTarget catchTarget;
    OffsetTarget offsetTarget;
    TypeArgumentTarget typeArgumentTarget;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            TargetInfo& instance);
};

struct Path {
    Path() {}
    ~Path() = default;

    uint8_t typePathKind;
    uint8_t typeArgIdx;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            Path& instance);
};

struct TypePath {
    TypePath() {}
    ~TypePath() = default;

    uint8_t pathLength;
    std::vector<Path> path;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            TypePath& instance);
};

struct TypeAnnotation {
    TypeAnnotation() {}
    ~TypeAnnotation() = default;

    uint8_t targetType;
    TargetInfo targetInfo;
    TypePath targetPath;
    uint16_t typeIdx;
    uint16_t numElemValPairs;
    std::vector<ElementValuePairs> elemValPairs;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            TypeAnnotation& instance);
};
}  // namespace TitaniumDecompiler
