#pragma once

#include <memory>
#include <vector>

#include "AttributeInnerStructures.h"
#include "ConstantPool.h"
#include "StackMapFrame.h"

namespace TitaniumDecompiler {

enum AttributeTypes {
    AnnotationDefaultType,
    BootstrapMethodsType,
    CodeType,
    ConstantValueType,
    DeprecatedType,
    EnclosingMethodType,
    ExceptionsType,
    InnerClassesType,
    LineNumberTableType,
    LocalVariableTableType,
    LocalVariableTypeTableType,
    MethodParametersType,
    // ModulesType,
    NestHostType,
    NestMembersType,
    // PermittedSubclassesType,
    // RecordType,

    RuntimeInvisibleAnnotationsType,
    RuntimeInvisibleParameterAnnotationsType,
    RuntimeInvisibleTypeAnnotationsType,
    RuntimeVisibleAnnotationsType,
    RuntimeVisibleParameterAnnotationsType,
    RuntimeVisibleTypeAnnotationsType,

    SignatureType,
    SourceFileType,
    StackMapTableType,
    SyntheticType,
    Unknown = 61
};

struct Attribute {
    virtual ~Attribute() = default;
    virtual void Deserialize(BigEndianStreamReader* deserializer,
                             Attribute& instance, const ConstantPool& cp) = 0;
};

struct AttributeInfo {
public:
    AttributeInfo()
        : m_AttributeNameIndex(0),
          m_AttributeLength(0),
          tag(AttributeTypes::Unknown),
          info(nullptr) {}
    AttributeInfo(uint16_t attributeNameIndex, uint32_t attributeLength,
                  Attribute* info)
        : m_AttributeNameIndex(attributeNameIndex),
          m_AttributeLength(attributeLength),
          info(info) {}

    ~AttributeInfo() = default;

    AttributeTypes GetAttrTypeFromName(const std::string& name);

    static void Deserialize(BigEndianStreamReader* deserializer,
                            AttributeInfo& instance,
                            const ConstantPool& constantPool);

private:
    std::shared_ptr<Attribute> CreateAttributeFromTag(AttributeInfo& instance);

public:
    uint16_t m_AttributeNameIndex;
    uint32_t m_AttributeLength;
    AttributeTypes tag;
    std::shared_ptr<Attribute> info;
};

struct ExceptionTable {
    ExceptionTable() {}
    ~ExceptionTable() = default;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            ExceptionTable& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.startPc);
        deserializer->ReadRawBigEndian<uint16_t>(instance.endPc);
        deserializer->ReadRawBigEndian<uint16_t>(instance.handlerPc);
        deserializer->ReadRawBigEndian<uint16_t>(instance.catchType);
    }

    uint16_t startPc;
    uint16_t endPc;
    uint16_t handlerPc;
    uint16_t catchType;
};

struct ConstantValue : Attribute {
    ConstantValue() {}
    ~ConstantValue() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        ConstantValue& constantValueInstance =
            static_cast<ConstantValue&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            constantValueInstance.constValIdx);
    };

    uint16_t constValIdx;
};

struct Code : Attribute {
    Code() {}
    ~Code() = default;
    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        Code& codeInstance = static_cast<Code&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(codeInstance.maxStack);
        deserializer->ReadRawBigEndian<uint16_t>(codeInstance.maxLocals);
        deserializer->ReadRawBigEndian<uint32_t>(codeInstance.codeLength);

        if (codeInstance.codeLength > 0 && codeInstance.codeLength < 65536) {
            deserializer->ReadArrayBigEndian(codeInstance.code,
                                             codeInstance.codeLength);
        }

        deserializer->ReadRawBigEndian<uint16_t>(
            codeInstance.exceptionTableLength);

        if (codeInstance.exceptionTableLength > 0) {
            deserializer->ReadArrayBigEndian(codeInstance.exceptionTable,
                                             codeInstance.exceptionTableLength);
        }

        deserializer->ReadRawBigEndian<uint16_t>(codeInstance.attributesCount);

        if (codeInstance.attributesCount > 0) {
            deserializer->ReadArrayBigEndian(codeInstance.attributes,
                                             codeInstance.attributesCount, cp);
        }
    }

    uint16_t maxStack;
    uint16_t maxLocals;
    uint32_t codeLength;
    std::vector<uint8_t> code;
    uint16_t exceptionTableLength;
    std::vector<ExceptionTable> exceptionTable;
    uint16_t attributesCount;
    std::vector<AttributeInfo> attributes;
};

struct LineNumberTable : Attribute {
    LineNumberTable() {}
    ~LineNumberTable() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        LineNumberTable& lineNumberTableInstance =
            static_cast<LineNumberTable&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            lineNumberTableInstance.lineNumberTableLength);
        if (lineNumberTableInstance.lineNumberTableLength > 0)
            deserializer->ReadArrayBigEndian(
                lineNumberTableInstance.lineNumberTable,
                lineNumberTableInstance.lineNumberTableLength);
    }

    uint16_t lineNumberTableLength;
    std::vector<LineNumTableItem> lineNumberTable;
};

struct LocalVariableTable : Attribute {
    LocalVariableTable() {}
    ~LocalVariableTable() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        LocalVariableTable& lineNumberTableInstance =
            static_cast<LocalVariableTable&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            lineNumberTableInstance.localVariableTableLength);
        if (lineNumberTableInstance.localVariableTableLength > 0) {
            deserializer->ReadArrayBigEndian(
                lineNumberTableInstance.localVariableTable,
                lineNumberTableInstance.localVariableTableLength);
        }
    }

    uint16_t localVariableTableLength;
    std::vector<LocalVariableTableEntry> localVariableTable;
};

struct StackMapTable : Attribute {
    StackMapTable() {}
    ~StackMapTable() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        StackMapTable& stackMapTableInstance =
            static_cast<StackMapTable&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            stackMapTableInstance.numOfEntries);
        if (stackMapTableInstance.numOfEntries > 0) {
            deserializer->ReadArrayBigEndian(
                stackMapTableInstance.entries,
                stackMapTableInstance.numOfEntries);
        }
    }

    std::string ResolveLocalVariable(int slot, uint16_t bytecodeOffset) const {
        for (const auto& frame : entries) {
            if (auto* fullFrame = dynamic_cast<StackMapAppendFrame*>(
                    frame.m_StackFrameType.get())) {
                if (fullFrame->offsetDelta == bytecodeOffset) {
                    if (slot < fullFrame->locals.size()) {
                        return fullFrame->locals[slot].ResolveType();
                    }
                    return "slot out of bounds";
                }
            }
            if (auto* fullFrame = dynamic_cast<StackMapFullFrame*>(
                    frame.m_StackFrameType.get())) {
                if (fullFrame->offsetDelta == bytecodeOffset) {
                    if (slot < fullFrame->locals.size()) {
                        return fullFrame->locals[slot].ResolveType();
                    }
                    return "slot out of bounds";
                }
            }
        }

        return "";
    }

    uint16_t numOfEntries;
    std::vector<StackMapFrame> entries;
};

struct ExceptionsAttr : Attribute {
    ExceptionsAttr() {}
    ~ExceptionsAttr() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        ExceptionsAttr& exceptionInstance =
            static_cast<ExceptionsAttr&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            exceptionInstance.numOfExceptions);
        if (exceptionInstance.numOfExceptions > 0)
            deserializer->ReadArrayBigEndian(
                exceptionInstance.exceptionIndexTable,
                exceptionInstance.numOfExceptions);
    }

    uint16_t numOfExceptions;
    std::vector<uint16_t> exceptionIndexTable;
};

struct InnerClasses : Attribute {
    InnerClasses() {}
    ~InnerClasses() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        InnerClasses& innerClassInstance = static_cast<InnerClasses&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            innerClassInstance.numOfClasses);
        if (innerClassInstance.numOfClasses > 0)
            deserializer->ReadArrayBigEndian(innerClassInstance.classes,
                                             innerClassInstance.numOfClasses);
    }

    uint16_t numOfClasses;
    std::vector<Classes> classes;
};

struct EnclosingMethod : Attribute {
    EnclosingMethod() {}
    ~EnclosingMethod() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        EnclosingMethod& enclosingMethodInstance =
            static_cast<EnclosingMethod&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            enclosingMethodInstance.classIdx);
        deserializer->ReadRawBigEndian<uint16_t>(
            enclosingMethodInstance.methodIdx);
    }

    uint16_t classIdx;
    uint16_t methodIdx;
};

struct Synthetic : Attribute {
    Synthetic() {}
    ~Synthetic() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        Synthetic& enclosingMethodInstance = static_cast<Synthetic&>(instance);
    }
};

struct Signature : Attribute {
    Signature() {}
    ~Signature() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        Signature& signatureInstance = static_cast<Signature&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            signatureInstance.signatureIdx);
    }

    uint16_t signatureIdx;
};

struct LocalVariableTypeTableAttr : Attribute {
    LocalVariableTypeTableAttr() {}
    ~LocalVariableTypeTableAttr() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        LocalVariableTypeTableAttr& localVarTypeTableInstance =
            static_cast<LocalVariableTypeTableAttr&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            localVarTypeTableInstance.localVarTypeTableLength);
        if (localVarTypeTableInstance.localVarTypeTableLength > 0)
            deserializer->ReadArrayBigEndian(
                localVarTypeTableInstance.localVarTypeTable,
                localVarTypeTableInstance.localVarTypeTableLength);
    }

    uint16_t localVarTypeTableLength;
    std::vector<LocalVariableTypeTable> localVarTypeTable;
};

struct Deprecated : Attribute {
    Deprecated() {}
    ~Deprecated() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        Deprecated& deprecatedInstance = static_cast<Deprecated&>(instance);
    }
};

struct BootstrapMethods : Attribute {
    BootstrapMethods() {}
    ~BootstrapMethods() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        BootstrapMethods& bootstrapMethodsInstance =
            static_cast<BootstrapMethods&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            bootstrapMethodsInstance.numBootstrapMethods);
        if (bootstrapMethodsInstance.numBootstrapMethods > 0)
            deserializer->ReadArrayBigEndian(
                bootstrapMethodsInstance.bootstrapsMethods,
                bootstrapMethodsInstance.numBootstrapMethods);
    }

    uint16_t numBootstrapMethods;
    std::vector<BootstrapMethodsInner> bootstrapsMethods;
};

struct MethodParameters : Attribute {
    MethodParameters() {}
    ~MethodParameters() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        MethodParameters& methodParametersInstance =
            static_cast<MethodParameters&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            methodParametersInstance.paramCount);
        if (methodParametersInstance.paramCount > 0)
            deserializer->ReadArrayBigEndian(
                methodParametersInstance.parameters,
                methodParametersInstance.paramCount);
    }

    uint16_t paramCount;
    std::vector<Parameters> parameters;
};

struct NestHost : Attribute {
    NestHost() {}
    ~NestHost() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        NestHost& nestHostInstance = static_cast<NestHost&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            nestHostInstance.mainClassIndex);
        // deserializer->ReadArrayBigEndian(nestHostInstance.parameters,
        // nestHostInstance.paramCount);
    }

    uint16_t mainClassIndex;
};

struct NestMembers : Attribute {
    NestMembers() {}
    ~NestMembers() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        NestMembers& nestMembersInstance = static_cast<NestMembers&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            nestMembersInstance.numOfClasses);
        if (nestMembersInstance.numOfClasses > 0)
            deserializer->ReadArrayBigEndian(nestMembersInstance.classes,
                                             nestMembersInstance.numOfClasses);
    }

    uint16_t numOfClasses;
    std::vector<uint16_t> classes;
};

struct SourceFile : Attribute {
    SourceFile() {}
    ~SourceFile() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        SourceFile& sourceFileInstance = static_cast<SourceFile&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            sourceFileInstance.sourceFileIndex);
    }

    uint16_t sourceFileIndex;
};

struct RuntimeVisibleAnnotations : Attribute {
    RuntimeVisibleAnnotations() {}
    ~RuntimeVisibleAnnotations() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        RuntimeVisibleAnnotations& runtimeVisAnnotationsInstance =
            static_cast<RuntimeVisibleAnnotations&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            runtimeVisAnnotationsInstance.numAnnotations);
        if (runtimeVisAnnotationsInstance.numAnnotations > 0) {
            deserializer->ReadArrayBigEndian(
                runtimeVisAnnotationsInstance.annotations,
                runtimeVisAnnotationsInstance.numAnnotations);
        }
    }

    uint16_t numAnnotations;
    std::vector<Annotation> annotations;
};

struct RuntimeInvisibleAnnotations : Attribute {
    RuntimeInvisibleAnnotations() {}
    ~RuntimeInvisibleAnnotations() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        RuntimeInvisibleAnnotations& runtimeInvisAnnotationsInstance =
            static_cast<RuntimeInvisibleAnnotations&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            runtimeInvisAnnotationsInstance.numAnnotations);
        if (runtimeInvisAnnotationsInstance.numAnnotations > 0)
            deserializer->ReadArrayBigEndian(
                runtimeInvisAnnotationsInstance.annotations,
                runtimeInvisAnnotationsInstance.numAnnotations);
    }

    uint16_t numAnnotations;
    std::vector<Annotation> annotations;
};

struct RuntimeVisibleParameterAnnotations : Attribute {
    RuntimeVisibleParameterAnnotations() {}
    ~RuntimeVisibleParameterAnnotations() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        RuntimeVisibleParameterAnnotations& runtimeVisParamInstance =
            static_cast<RuntimeVisibleParameterAnnotations&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            runtimeVisParamInstance.numParameters);
        if (runtimeVisParamInstance.numParameters > 0)
            deserializer->ReadArrayBigEndian(
                runtimeVisParamInstance.paramAnnotations,
                runtimeVisParamInstance.numParameters);
    }

    uint16_t numParameters;
    std::vector<ParameterAnnotations> paramAnnotations;
};

struct RuntimeInvisibleParameterAnnotations : Attribute {
    RuntimeInvisibleParameterAnnotations() {}
    ~RuntimeInvisibleParameterAnnotations() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        RuntimeInvisibleParameterAnnotations& runtimeInvisParamInstance =
            static_cast<RuntimeInvisibleParameterAnnotations&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            runtimeInvisParamInstance.numParameters);
        if (runtimeInvisParamInstance.numParameters > 0)
            deserializer->ReadArrayBigEndian(
                runtimeInvisParamInstance.paramAnnotations,
                runtimeInvisParamInstance.numParameters);
    }

    uint16_t numParameters;
    std::vector<ParameterAnnotations> paramAnnotations;
};

struct RuntimeVisibleTypeAnnotations : Attribute {
    RuntimeVisibleTypeAnnotations() {}
    ~RuntimeVisibleTypeAnnotations() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        RuntimeVisibleTypeAnnotations& runtimeVisibleTypeAnnotationsInstance =
            static_cast<RuntimeVisibleTypeAnnotations&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            runtimeVisibleTypeAnnotationsInstance.numAnnotations);
        if (runtimeVisibleTypeAnnotationsInstance.numAnnotations > 0)
            deserializer->ReadArrayBigEndian(
                runtimeVisibleTypeAnnotationsInstance.annotations,
                runtimeVisibleTypeAnnotationsInstance.numAnnotations);
    }

    uint16_t numAnnotations;
    std::vector<TypeAnnotation> annotations;
};

struct RuntimeInvisibleTypeAnnotations : Attribute {
    RuntimeInvisibleTypeAnnotations() {}
    ~RuntimeInvisibleTypeAnnotations() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        RuntimeInvisibleTypeAnnotations& runtimeInvisTypeAnnotationsInstance =
            static_cast<RuntimeInvisibleTypeAnnotations&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            runtimeInvisTypeAnnotationsInstance.numParameters);
        if (runtimeInvisTypeAnnotationsInstance.numParameters > 0)
            deserializer->ReadArrayBigEndian(
                runtimeInvisTypeAnnotationsInstance.paramAnnotations,
                runtimeInvisTypeAnnotationsInstance.numParameters);
    }

    uint16_t numParameters;
    std::vector<ParameterAnnotations> paramAnnotations;
};

struct AnnotationDefault : Attribute {
    AnnotationDefault() {}
    ~AnnotationDefault() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance,
                     const ConstantPool& cp) override {
        [[maybe_unused]] AnnotationDefault& annotationDefaultInstance =
            static_cast<AnnotationDefault&>(instance);
        deserializer->ReadObject(defaultVal);
    }

    ElementValue defaultVal;
};

// struct Unknown {
//     Unknown(FileReader& reader, uint32_t value);
//     std::vector<uint8_t> info;
// };
}  // namespace TitaniumDecompiler
