# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)



DESCRIPTOR = descriptor.FileDescriptor(
  name='batchtank.proto',
  package='batchtank_messages',
  serialized_pb='\n\x0f\x62\x61tchtank.proto\x12\x12\x62\x61tchtank_messages\"\x91\x02\n\x0b\x42\x61seMessage\x12*\n\x06sample\x18\x01 \x03(\x0b\x32\x1a.batchtank_messages.Sample\x12\x31\n\x06signal\x18\x02 \x03(\x0b\x32!.batchtank_messages.ControlSignal\x12.\n\x08register\x18\x03 \x01(\x0b\x32\x1c.batchtank_messages.Register\x12-\n\tgetSensor\x18\x04 \x03(\x0e\x32\x1a.batchtank_messages.Sensor\x12-\n\tgetOutput\x18\x05 \x03(\x0e\x32\x1a.batchtank_messages.Output\x12\x15\n\rendConnection\x18\x06 \x01(\x08\"H\n\x08Register\x12(\n\x04type\x18\x01 \x03(\x0e\x32\x1a.batchtank_messages.Sensor\x12\x12\n\nperiodTime\x18\x02 \x02(\x04\"U\n\rControlSignal\x12\r\n\x05value\x18\x01 \x02(\x05\x12\x0b\n\x03ref\x18\x02 \x02(\x05\x12(\n\x04type\x18\x03 \x02(\x0e\x32\x1a.batchtank_messages.Output\"A\n\x06Sample\x12\r\n\x05value\x18\x01 \x02(\x05\x12(\n\x04type\x18\x02 \x02(\x0e\x32\x1a.batchtank_messages.Sensor*t\n\x06Sensor\x12\x08\n\x04TEMP\x10\x00\x12\t\n\x05LEVEL\x10\x01\x12\x10\n\x0cIN_PUMP_RATE\x10\x02\x12\x11\n\rOUT_PUMP_RATE\x10\x03\x12\x0f\n\x0bHEATER_RATE\x10\x04\x12\x0e\n\nMIXER_RATE\x10\x05\x12\x0f\n\x0b\x43OOLER_RATE\x10\x06*F\n\x06Output\x12\n\n\x06HEATER\x10\x00\x12\n\n\x06\x43OOLER\x10\x01\x12\x0b\n\x07IN_PUMP\x10\x02\x12\x0c\n\x08OUT_PUMP\x10\x03\x12\t\n\x05MIXER\x10\x04\x42\x02H\x03')

_SENSOR = descriptor.EnumDescriptor(
  name='Sensor',
  full_name='batchtank_messages.Sensor',
  filename=None,
  file=DESCRIPTOR,
  values=[
    descriptor.EnumValueDescriptor(
      name='TEMP', index=0, number=0,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='LEVEL', index=1, number=1,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='IN_PUMP_RATE', index=2, number=2,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='OUT_PUMP_RATE', index=3, number=3,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='HEATER_RATE', index=4, number=4,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='MIXER_RATE', index=5, number=5,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='COOLER_RATE', index=6, number=6,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=543,
  serialized_end=659,
)


_OUTPUT = descriptor.EnumDescriptor(
  name='Output',
  full_name='batchtank_messages.Output',
  filename=None,
  file=DESCRIPTOR,
  values=[
    descriptor.EnumValueDescriptor(
      name='HEATER', index=0, number=0,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='COOLER', index=1, number=1,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='IN_PUMP', index=2, number=2,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='OUT_PUMP', index=3, number=3,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='MIXER', index=4, number=4,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=661,
  serialized_end=731,
)


TEMP = 0
LEVEL = 1
IN_PUMP_RATE = 2
OUT_PUMP_RATE = 3
HEATER_RATE = 4
MIXER_RATE = 5
COOLER_RATE = 6
HEATER = 0
COOLER = 1
IN_PUMP = 2
OUT_PUMP = 3
MIXER = 4



_BASEMESSAGE = descriptor.Descriptor(
  name='BaseMessage',
  full_name='batchtank_messages.BaseMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='sample', full_name='batchtank_messages.BaseMessage.sample', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='signal', full_name='batchtank_messages.BaseMessage.signal', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='register', full_name='batchtank_messages.BaseMessage.register', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='getSensor', full_name='batchtank_messages.BaseMessage.getSensor', index=3,
      number=4, type=14, cpp_type=8, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='getOutput', full_name='batchtank_messages.BaseMessage.getOutput', index=4,
      number=5, type=14, cpp_type=8, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='endConnection', full_name='batchtank_messages.BaseMessage.endConnection', index=5,
      number=6, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=40,
  serialized_end=313,
)


_REGISTER = descriptor.Descriptor(
  name='Register',
  full_name='batchtank_messages.Register',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='type', full_name='batchtank_messages.Register.type', index=0,
      number=1, type=14, cpp_type=8, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='periodTime', full_name='batchtank_messages.Register.periodTime', index=1,
      number=2, type=4, cpp_type=4, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=315,
  serialized_end=387,
)


_CONTROLSIGNAL = descriptor.Descriptor(
  name='ControlSignal',
  full_name='batchtank_messages.ControlSignal',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='value', full_name='batchtank_messages.ControlSignal.value', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='ref', full_name='batchtank_messages.ControlSignal.ref', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='type', full_name='batchtank_messages.ControlSignal.type', index=2,
      number=3, type=14, cpp_type=8, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=389,
  serialized_end=474,
)


_SAMPLE = descriptor.Descriptor(
  name='Sample',
  full_name='batchtank_messages.Sample',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='value', full_name='batchtank_messages.Sample.value', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='type', full_name='batchtank_messages.Sample.type', index=1,
      number=2, type=14, cpp_type=8, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=476,
  serialized_end=541,
)

_BASEMESSAGE.fields_by_name['sample'].message_type = _SAMPLE
_BASEMESSAGE.fields_by_name['signal'].message_type = _CONTROLSIGNAL
_BASEMESSAGE.fields_by_name['register'].message_type = _REGISTER
_BASEMESSAGE.fields_by_name['getSensor'].enum_type = _SENSOR
_BASEMESSAGE.fields_by_name['getOutput'].enum_type = _OUTPUT
_REGISTER.fields_by_name['type'].enum_type = _SENSOR
_CONTROLSIGNAL.fields_by_name['type'].enum_type = _OUTPUT
_SAMPLE.fields_by_name['type'].enum_type = _SENSOR
DESCRIPTOR.message_types_by_name['BaseMessage'] = _BASEMESSAGE
DESCRIPTOR.message_types_by_name['Register'] = _REGISTER
DESCRIPTOR.message_types_by_name['ControlSignal'] = _CONTROLSIGNAL
DESCRIPTOR.message_types_by_name['Sample'] = _SAMPLE

class BaseMessage(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BASEMESSAGE
  
  # @@protoc_insertion_point(class_scope:batchtank_messages.BaseMessage)

class Register(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _REGISTER
  
  # @@protoc_insertion_point(class_scope:batchtank_messages.Register)

class ControlSignal(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _CONTROLSIGNAL
  
  # @@protoc_insertion_point(class_scope:batchtank_messages.ControlSignal)

class Sample(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SAMPLE
  
  # @@protoc_insertion_point(class_scope:batchtank_messages.Sample)

# @@protoc_insertion_point(module_scope)
