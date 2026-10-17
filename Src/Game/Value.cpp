#include "stdafx.h"
#include "Value.h"

Value::Value ()
{
	m_type = VT_Empty;
}

Value::~Value ()
{
	SetEmpty();
}

ValueType Value::GetType ()
{
	return m_type;
}

Value* Value::Clone ()
{
	Value* result = new Value;
	result->Set(this);
	return result;
}

bool Value::GetBool ()
{
	CheckType(VT_Bool);
	return m_bool;
}

int Value::GetInt ()
{
	CheckType(VT_Int);
	return m_int;
}

float Value::GetFloat ()
{
	CheckType(VT_Float);
	return m_float;
}

stringc Value::GetStringC ()
{
	CheckType(VT_String);
	return m_string->c_str();
}

stringw Value::GetStringW ()
{
	CheckType(VT_String);
	return m_string->c_str();
}

Value* Value::SetEmpty ()
{
	if (m_type == VT_String)
	{
		delete m_string;
	}
	if (m_type == VT_Map)
	{
		MapClear();
		delete m_map;
	}
	if (m_type == VT_Array)
	{
		ArrayClear();
		delete m_array;
	}
	m_type = VT_Empty;
	return this;
}

Value* Value::Set (bool value)
{
	if (m_type != VT_Bool)
	{
		SetEmpty();
		m_type = VT_Bool;
	}
	m_bool = value;
	return this;
}

Value* Value::Set (int value)
{
	if (m_type != VT_Int)
	{
		SetEmpty();
		m_type = VT_Int;
	}
	m_int = value;
	return this;
}

Value* Value::Set (float value)
{
	if (m_type != VT_Float)
	{
		SetEmpty();
		m_type = VT_Float;
	}
	m_float = value;
	return this;
}

Value* Value::Set (const char* value)
{
	return Set(stringc(value));
}

Value* Value::Set (const wchar_t* value)
{
	return Set(stringw(value));
}

Value* Value::Set (const stringc& value)
{
	if (m_type != VT_String)
	{
		SetEmpty();
		m_type = VT_String;
		m_string = new stringw(value.c_str());
	}
	else
	{
		*m_string = value.c_str();
	}
	return this;
}

Value* Value::Set (const stringw& value)
{
	if (m_type != VT_String)
	{
		SetEmpty();
		m_type = VT_String;
		m_string = new stringw(value.c_str());
	}
	else
	{
		*m_string = value.c_str();
	}
	return this;
}

Value* Value::SetMap ()
{
	if (m_type != VT_Map)
	{
		SetEmpty();
		m_type = VT_Map;
		m_map = new map<stringc, Value*>;
	}
	return this;
}

Value* Value::Set (Value* val)
{
	SetEmpty();

	switch (val->GetType())
	{
		case VT_Bool:
			Set(val->GetBool());
			break;

		case VT_Int:
			Set(val->GetInt());
			break;

		case VT_Float:
			Set(val->GetFloat());
			break;

		case VT_String:
			Set(val->GetStringW());
			break;

		case VT_Map:
			SetMap();
			for (MapIterator it = val->MapGetIterator(); !it.atEnd(); it++)
			{
				MapGet(it->getKey())->Set(it->getValue());
			}
			break;

		case VT_Array:
			SetArray();
			for (int i = 0; i < val->ArraySize(); ++i)
			{
				ArrayPushBack()->Set(val->ArrayGet(i));
			}
			break;
	}

	return this;
}

Value::MapIterator Value::MapGetIterator ()
{
	CheckType(VT_Map);
	return m_map->getIterator();
}

bool Value::MapExists (const stringc& key)
{
	CheckType(VT_Map);
	return m_map->find(key) != 0;
}

Value* Value::MapGet (const stringc& key)
{
	CheckType(VT_Map);
	map<stringc, Value*>::Node* node = m_map->find(key);
	if (node == 0)
	{
		Value* result = new Value;
		m_map->insert(key, result);
		return result;
	}
	return node->getValue();
}

void Value::MapErase (const stringc& key)
{
	CheckType(VT_Map);
	map<stringc, Value*>::Node* node = m_map->find(key);
	if (node != 0)
	{
		delete node->getValue();
		m_map->remove(node);
	}
}

void Value::MapClear ()
{
	CheckType(VT_Map);
	for (MapIterator it = MapGetIterator(); !it.atEnd(); it = MapGetIterator())
	{
		m_map->remove(it.getNode());
	}
}

bool Value::MapGetBool (const stringc& key)
{
	return MapGet(key)->GetBool();
}

int Value::MapGetInt (const stringc& key)
{
	return MapGet(key)->GetInt();
}

float Value::MapGetFloat (const stringc& key)
{
	return MapGet(key)->GetFloat();
}

stringc Value::MapGetStringC (const stringc& key)
{
	return MapGet(key)->GetStringC();
}

stringw Value::MapGetStringW (const stringc& key)
{
	return MapGet(key)->GetStringW();
}

Value* Value::MapSet (const stringc& key, bool value)
{
	return MapGet(key)->Set(value);
}

Value* Value::MapSet (const stringc& key, int value)
{
	return MapGet(key)->Set(value);
}

Value* Value::MapSet (const stringc& key, float value)
{
	return MapGet(key)->Set(value);
}

Value* Value::MapSet (const stringc& key, const char* value)
{
	return MapGet(key)->Set(value);
}

Value* Value::MapSet (const stringc& key, const wchar_t* value)
{
	return MapGet(key)->Set(value);
}

Value* Value::MapSet (const stringc& key, const stringc& value)
{
	return MapGet(key)->Set(value);
}

Value* Value::MapSet (const stringc& key, const stringw& value)
{
	return MapGet(key)->Set(value);
}
Value* Value::MapSet (const stringc& key, Value* value)
{
	return MapGet(key)->Set(value);
}

Value* Value::MapSetMap (const stringc& key)
{
	return MapGet(key)->SetMap();
}

Value* Value::MapSetArray (const stringc& key)
{
	return MapGet(key)->SetArray();
}

Value* Value::SetArray ()
{
	if (m_type != VT_Array)
	{
		SetEmpty();
		m_type = VT_Array;
		m_array = new array<Value*>;
	}
	return this;
}

int Value::ArraySize ()
{
	CheckType(VT_Array);
	return m_array->size();
}

Value* Value::ArrayGet (int index)
{
	CheckType(VT_Array);
	return (*m_array)[index];
}

Value* Value::ArrayPushBack ()
{
	CheckType(VT_Array);
	m_array->push_back(new Value);
	return (*m_array)[m_array->size() - 1];
}

void Value::ArrayPopBack ()
{
	CheckType(VT_Array);
	delete (*m_array)[m_array->size() - 1];
	m_array->erase(m_array->size() - 1);
}

void Value::ArrayClear ()
{
	CheckType(VT_Array);
	while (ArraySize() > 0)
	{
		ArrayPopBack();
	}
}

bool Value::ArrayGetBool (int index)
{
	return ArrayGet(index)->GetBool();
}

int Value::ArrayGetInt (int index)
{
	return ArrayGet(index)->GetInt();
}

float Value::ArrayGetFloat (int index)
{
	return ArrayGet(index)->GetFloat();
}

stringc Value::ArrayGetStringC (int index)
{
	return ArrayGet(index)->GetStringC();
}

stringw Value::ArrayGetStringW (int index)
{
	return ArrayGet(index)->GetStringW();
}

Value* Value::ArraySet (int index, bool value)
{
	return ArrayGet(index)->Set(value);
}

Value* Value::ArraySet (int index, int value)
{
	return ArrayGet(index)->Set(value);
}

Value* Value::ArraySet (int index, float value)
{
	return ArrayGet(index)->Set(value);
}

Value* Value::ArraySet (int index, const char* value)
{
	return ArrayGet(index)->Set(value);
}

Value* Value::ArraySet (int index, const wchar_t* value)
{
	return ArrayGet(index)->Set(value);
}

Value* Value::ArraySet (int index, const stringc& value)
{
	return ArrayGet(index)->Set(value);
}

Value* Value::ArraySet (int index, const stringw& value)
{
	return ArrayGet(index)->Set(value);
}

Value* Value::ArraySet (int index, Value* value)
{
	return ArrayGet(index)->Set(value);
}

Value* Value::ArraySetMap (int index)
{
	return ArrayGet(index)->SetMap();
}

Value* Value::ArraySetArray (int index)
{
	return ArrayGet(index)->SetArray();
}

Value* Value::ArrayPushBack (bool value)
{
	return ArrayPushBack()->Set(value);
}

Value* Value::ArrayPushBack (int value)
{
	return ArrayPushBack()->Set(value);
}

Value* Value::ArrayPushBack (float value)
{
	return ArrayPushBack()->Set(value);
}

Value* Value::ArrayPushBack (const char* value)
{
	return ArrayPushBack()->Set(value);
}

Value* Value::ArrayPushBack (const wchar_t* value)
{
	return ArrayPushBack()->Set(value);
}

Value* Value::ArrayPushBack (const stringc& value)
{
	return ArrayPushBack()->Set(value);
}

Value* Value::ArrayPushBack (const stringw& value)
{
	return ArrayPushBack()->Set(value);
}

Value* Value::ArrayPushBack (Value* value)
{
	return ArrayPushBack()->Set(value);
}

Value* Value::ArrayPushBackMap ()
{
	return ArrayPushBack()->SetMap();
}

Value* Value::ArrayPushBackArray ()
{
	return ArrayPushBack()->SetArray();
}

Value* Value::Read (const stringc& file)
{
	IXMLReader* reader = __IRR_DEVICE()->getFileSystem()->createXMLReader(file.c_str());
	if (reader)
	{
		Value* result = new Value;
		result->DoRead(reader, 0, true);
		reader->drop();
		return result;
	}
	else
	{
		return 0;
	}
}

void Value::Write (const stringc& file)
{
	IXMLWriter* writer = __IRR_DEVICE()->getFileSystem()->createXMLWriter(file.c_str());
	if (writer)
	{
		DoWrite(writer, 0);
		writer->drop();
	}
}

void Value::DoRead (IXMLReader* reader, stringc* keyResult, bool doRead)
{
	SetEmpty();
	if (doRead)
	{
		while (reader->read() && reader->getNodeType() != EXN_ELEMENT)
		{
		};
	}
	
	if (keyResult)
	{
		*keyResult = reader->getAttributeValue(L"name");
	}

	const wchar_t* type = reader->getNodeName();
	bool readEnd = true;
	if (wcscmp(type, L"bool") == 0)
	{
		Set(wcscmp(reader->getAttributeValue(L"value"), L"true") == 0);
	}
	else if (wcscmp(type, L"int") == 0)
	{
		Set(reader->getAttributeValueAsInt(L"value"));
	}
	else if (wcscmp(type, L"float") == 0)
	{
		Set(reader->getAttributeValueAsFloat(L"value"));
	}
	else if (wcscmp(type, L"string") == 0)
	{
		Set(reader->getAttributeValue(L"value"));
	}
	else if (wcscmp(type, L"map") == 0)
	{
		SetMap();
		// try to read a child or the closing tag
		while (reader->read())
		{
			if (reader->getNodeType() == EXN_ELEMENT)
			{
				Value* val = new Value;
				stringc key;
				val->DoRead(reader, &key, false);
				m_map->insert(key, val);
			}
			else if (reader->getNodeType() == EXN_ELEMENT_END)
			{
				assert(wcscmp(reader->getNodeName(), L"map") == 0);
				break;
			}
		}
		readEnd = false;
	}
	else if (wcscmp(type, L"array") == 0)
	{
		SetArray();
		// try to read a child or the closing tag
		while (reader->read())
		{
			if (reader->getNodeType() == EXN_ELEMENT)
			{
				ArrayPushBack()->DoRead(reader, 0, false);
			}
			else if (reader->getNodeType() == EXN_ELEMENT_END)
			{
				assert(wcscmp(reader->getNodeName(), L"array") == 0);
				break;
			}
		}
		readEnd = false;
	}
	else
	{
		assert(wcscmp(reader->getNodeName(), L"empty") == 0);
	}

	if (readEnd)
	{
		reader->read();
		assert(reader->getNodeType() == EXN_ELEMENT_END);
	}
}

void WriteElement (IXMLWriter* writer, const wchar_t* name, bool empty, const wchar_t* key, const wchar_t* value)
{
	if (key)
	{
		if (value)
		{
			writer->writeElement(name, empty, L"name", key, L"value", value);
		}
		else
		{
			writer->writeElement(name, empty, L"name", key);
		}
	}
	else
	{
		if (value)
		{
			writer->writeElement(name, empty, L"value", value);
		}
		else
		{
			writer->writeElement(name, empty);
		}
	}
}

void Value::DoWrite (IXMLWriter* writer, const wchar_t* key)
{
	switch (m_type)
	{
		case VT_Empty:
			WriteElement(writer, L"empty", true, key, 0);
			break;

		case VT_Bool:
			WriteElement(writer, L"bool", true, key, m_bool ? L"true" : L"false");
			break;

		case VT_Int:
			WriteElement(writer, L"int", true, key, stringw(m_int).c_str());
			break;

		case VT_Float:
			WriteElement(writer, L"float", true, key, stringw(m_float).c_str());
			break;

		case VT_String:
			WriteElement(writer, L"string", true, key, m_string->c_str());
			break;

		case VT_Map:
			writer->writeElement(L"map", false, key, 0);
			writer->writeLineBreak();
			for (MapIterator it = MapGetIterator(); !it.atEnd(); it++)
			{
				it->getValue()->DoWrite(writer, stringw(it->getKey().c_str()).c_str());
			}
			writer->writeClosingTag(L"map");
			break;

		case VT_Array:
			WriteElement(writer, L"array", false, key, 0);
			writer->writeLineBreak();
			for (int i = 0; i < ArraySize(); ++i)
			{
				ArrayGet(i)->DoWrite(writer, 0);
			}
			writer->writeClosingTag(L"array");
			break;
	}
	writer->writeLineBreak();
}

void Value::CheckType (ValueType type)
{
	if (m_type != type)
	{
		// CRASH! :)
		int* a = 0;
		*a = 0;
	}
}
