#include "AttributeInnerStructures.h"

#include <iostream>

namespace TitaniumDecompiler {

void LineNumTableItem::Deserialize(BigEndianStreamReader *deserializer, LineNumTableItem &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.startPc);
    deserializer->ReadRawBigEndian<uint16_t>(instance.lineNumber);
}

void LocalVariableTableEntry::Deserialize(BigEndianStreamReader *deserializer, LocalVariableTableEntry &instance) {
    deserializer->ReadRawBigEndian(instance.startPc);
    deserializer->ReadRawBigEndian(instance.length);
    deserializer->ReadRawBigEndian(instance.nameIndex);
    deserializer->ReadRawBigEndian(instance.descIndex);
    deserializer->ReadRawBigEndian(instance.index);
}

void Classes::Deserialize(BigEndianStreamReader *deserializer, Classes &instance) {
    deserializer->ReadRawBigEndian(instance.innerClassInfoIdx);
    deserializer->ReadRawBigEndian(instance.outerClassInfoIdx);
    deserializer->ReadRawBigEndian(instance.innerNameIdx);
    deserializer->ReadRawBigEndian(instance.innerClassAccessFlags);
}

void LocalVariableTypeTable::Deserialize(BigEndianStreamReader *deserializer, LocalVariableTypeTable &instance) {
    deserializer->ReadRawBigEndian(instance.startPc);
    deserializer->ReadRawBigEndian(instance.length);
    deserializer->ReadRawBigEndian(instance.nameIdx);
    deserializer->ReadRawBigEndian(instance.sigIdx);
    deserializer->ReadRawBigEndian(instance.idx);
}

void BootstrapMethodsInner::Deserialize(BigEndianStreamReader *deserializer, BootstrapMethodsInner &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.bootstrapMethodRef);
    deserializer->ReadRawBigEndian<uint16_t>(instance.numBootstrapArgs);

    if (instance.numBootstrapArgs > 0) {
        deserializer->ReadArrayBigEndian(instance.bootstrapArgs, instance.numBootstrapArgs);
    }
}

void Parameters::Deserialize(BigEndianStreamReader *deserializer, Parameters &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.nameIdx);
    deserializer->ReadRawBigEndian<uint16_t>(instance.accessFlags);
}

void EnumConstValue::Deserialize(BigEndianStreamReader *deserializer, EnumConstValue &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.typeNameIdx);
    deserializer->ReadRawBigEndian<uint16_t>(instance.constNameIdx);
}

void ArrayValue::Deserialize(BigEndianStreamReader *deserializer, ArrayValue &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.numVals);
    if (instance.numVals > 0) {
        deserializer->ReadArrayBigEndian(instance.values, instance.numVals);
    }
}

void ElementValue::Deserialize(BigEndianStreamReader *deserializer, ElementValue &instance) {
    deserializer->ReadRawBigEndian<uint8_t>(instance.tag);
    deserializer->ReadObject(instance.value);
}

void ElementValue::Value::Deserialize(BigEndianStreamReader *deserializer, Value &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.constValIdx);
    deserializer->ReadObject(instance.enumConstVal);
    deserializer->ReadRawBigEndian<uint16_t>(instance.classInfoIdx);
    instance.annotationVal = std::make_shared<Annotation>();
    deserializer->ReadObject<Annotation>(*instance.annotationVal);
    instance.arrVal = std::make_shared<ArrayValue>();
    deserializer->ReadObject<ArrayValue>(*instance.arrVal);
}

void ElementValuePairs::Deserialize(BigEndianStreamReader *deserializer, ElementValuePairs &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.elemNameIdx);
    deserializer->ReadObject(instance.value);
}

void Annotation::Deserialize(BigEndianStreamReader *deserializer, Annotation &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.typeIdx);
    deserializer->ReadRawBigEndian<uint16_t>(instance.numElemValPairs);

    if (instance.numElemValPairs > 0) {
        deserializer->ReadArrayBigEndian(instance.elemValPairs, instance.numElemValPairs);
    }
}

void ParameterAnnotations::Deserialize(BigEndianStreamReader *deserializer, ParameterAnnotations &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.numAnnotations);

    if (instance.numAnnotations > 0) {
        deserializer->ReadArrayBigEndian(instance.annotations, instance.numAnnotations);
    }
}

void Path::Deserialize(BigEndianStreamReader *deserializer, Path &instance) {
    deserializer->ReadRawBigEndian<uint8_t>(instance.typePathKind);
    deserializer->ReadRawBigEndian<uint8_t>(instance.typeArgIdx);
}

void TypeParameterTarget::Deserialize(BigEndianStreamReader *deserializer, TypeParameterTarget &instance) {
    deserializer->ReadRawBigEndian<uint8_t>(instance.typeParamIdx);
}

void SuperTypeTarget::Deserialize(BigEndianStreamReader *deserializer, SuperTypeTarget &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.superTypeIdx);
}

void TypeParameterBoundTarget::Deserialize(BigEndianStreamReader *deserializer, TypeParameterBoundTarget &instance) {
    deserializer->ReadRawBigEndian<uint8_t>(instance.typeParamIdx);
    deserializer->ReadRawBigEndian<uint8_t>(instance.boundIdx);
}

void EmptyTarget::Deserialize(BigEndianStreamReader *deserializer, EmptyTarget &instance) {}

void FormalParameterTarget::Deserialize(BigEndianStreamReader *deserializer, FormalParameterTarget &instance) {
    deserializer->ReadRawBigEndian<uint8_t>(instance.formalParamIdx);
}

void ThrowsTarget::Deserialize(BigEndianStreamReader *deserializer, ThrowsTarget &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.throwsTypeIdx);
}

void LocalVarTargetTable::Deserialize(BigEndianStreamReader *deserializer, LocalVarTargetTable &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.startPc);
    deserializer->ReadRawBigEndian<uint16_t>(instance.length);
    deserializer->ReadRawBigEndian<uint16_t>(instance.index);
}

void LocalVarTarget::Deserialize(BigEndianStreamReader *deserializer, LocalVarTarget &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.tableLength);
    if (instance.tableLength > 0) {
        deserializer->ReadArrayBigEndian(instance.table, instance.tableLength);
    }
}

void CatchTarget::Deserialize(BigEndianStreamReader *deserializer, CatchTarget &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.exceptionTableIdx);
}

void OffsetTarget::Deserialize(BigEndianStreamReader *deserializer, OffsetTarget &instance) { deserializer->ReadRawBigEndian<uint16_t>(instance.offset); }

void TypeArgumentTarget::Deserialize(BigEndianStreamReader *deserializer, TypeArgumentTarget &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.offset);
    deserializer->ReadRawBigEndian<uint8_t>(instance.typeArgIdx);
}

void TargetInfo::Deserialize(BigEndianStreamReader *deserializer, TargetInfo &instance) {
    deserializer->ReadObject(instance.typeParamTarget);
    deserializer->ReadObject(instance.superTypeTarget);
    deserializer->ReadObject(instance.typeParamBoundTarget);
    deserializer->ReadObject(instance.emptyTarget);
    deserializer->ReadObject(instance.formalParameterTarget);
    deserializer->ReadObject(instance.throwsTarget);
    deserializer->ReadObject(instance.localvarTarget);
    deserializer->ReadObject(instance.catchTarget);
    deserializer->ReadObject(instance.offsetTarget);
    deserializer->ReadObject(instance.typeArgumentTarget);
}

void TypePath::Deserialize(BigEndianStreamReader *deserializer, TypePath &instance) {
    deserializer->ReadRawBigEndian<uint8_t>(instance.pathLength);

    if (instance.pathLength > 0) {
        deserializer->ReadArrayBigEndian(instance.path, instance.pathLength);
    }
}

void TypeAnnotation::Deserialize(BigEndianStreamReader *deserializer, TypeAnnotation &instance) {
    deserializer->ReadRawBigEndian<uint8_t>(instance.targetType);
    deserializer->ReadObject(instance.targetInfo);
    deserializer->ReadObject(instance.targetPath);
    deserializer->ReadRawBigEndian<uint16_t>(instance.typeIdx);
    deserializer->ReadRawBigEndian<uint16_t>(instance.numElemValPairs);

    if (instance.numElemValPairs > 0) {
        deserializer->ReadArrayBigEndian(instance.elemValPairs, instance.numElemValPairs);
    }
}

}  // namespace TitaniumDecompiler
