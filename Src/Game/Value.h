#pragma once

#include "Core/irrlicht.h"

enum ValueType
{
	VT_Empty,
	VT_Bool,
	VT_Int,
	VT_Float,
	VT_String,
	VT_Map,
	VT_Array
};

// This class represents a single value whose type may vary.
// Apart from the simple values empty, bool, int, float, string, a Value object can also
// represent a map from string to other Value objects or an array of Value objects. This
// way entire trees of values can be represented. Value objects can be serialized to XML.
class Value
{
	// No copy constructor. You should only allocate Value objects on the heap and make copies with Clone.
	Value (const Value&);

public:
	// Initializes the value as empty.
	Value ();
	~Value ();

	// ------------------------------------------------------------------------------------------------------
	// Returns the value type. Check this before calling the GetXXX functions or they will CRASH!
	ValueType			GetType();
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// Creates a deep copy of this value and its children.
	Value*				Clone ();
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// Simple value access -- these functions will CRASH if the value has the wrong type!
	bool				GetBool ();
	int					GetInt ();
	float				GetFloat ();
	stringc				GetStringC ();
	stringw				GetStringW ();
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// Simple value setting. These functions will delete the old value if it has a different type.
	Value*				SetEmpty ();
	Value*				Set (bool value);
	Value*				Set (int value);
	Value*				Set (float value);
	Value*				Set (const char* value);
	Value*				Set (const wchar_t* value);
	Value*				Set (const stringc& value);
	Value*				Set (const stringw& value);
	Value*				Set (Value* val);
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// Working with maps -- all functions except SetMap will CRASH if the value is not a map.
	typedef map<stringc, Value*>::Iterator MapIterator;
	Value*				SetMap (); // will change the type to a map if it isn't already
	MapIterator			MapGetIterator ();
	bool				MapExists (const stringc& key);
	Value*				MapGet (const stringc& key); // will create an empty value if the key doesn't exist
	void				MapErase (const stringc& key);
	void				MapClear ();
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// Working with arrays -- all functions except SetArray will CRASH if the value is not an array.
	Value*				SetArray (); // will change the type to an array if it isn't already
	int					ArraySize ();
	Value*				ArrayGet (int index);
	Value*				ArrayPushBack (); // inserts a new empty value at the end
	void				ArrayPopBack (); // removes a value from the end
	void				ArrayErase (int index);
	void				ArrayClear ();
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// Utility functions for working with maps.
	// Same as MapGet(key)->GetXXX()
	bool				MapGetBool    (const stringc& key);
	int					MapGetInt     (const stringc& key);
	float				MapGetFloat   (const stringc& key);
	stringc				MapGetStringC (const stringc& key);
	stringw				MapGetStringW (const stringc& key);
	SColor				MapGetColor   (const stringc& key);
	// Same as MapGet(key)->Set(value)
	Value*				MapSet (const stringc& key, bool value);
	Value*				MapSet (const stringc& key, int value);
	Value*				MapSet (const stringc& key, float value);
	Value*				MapSet (const stringc& key, const char* value);
	Value*				MapSet (const stringc& key, const wchar_t* value);
	Value*				MapSet (const stringc& key, const stringc& value);
	Value*				MapSet (const stringc& key, const stringw& value);
	Value*				MapSet (const stringc& key, Value* value);
	Value*				MapSetMap   (const stringc& key);
	Value*				MapSetArray (const stringc& key);
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// Utility functions for working with arrays.
	// Same as ArrayGet(index)->GetXXX()
	bool				ArrayGetBool    (int index);
	int					ArrayGetInt     (int index);
	float				ArrayGetFloat   (int index);
	stringc				ArrayGetStringC (int index);
	stringw				ArrayGetStringW (int index);
	SColor				ArrayGetColor   (int index);
	// Same as ArrayGet(index)->Set(value)
	Value*				ArraySet (int index, bool value);
	Value*				ArraySet (int index, int value);
	Value*				ArraySet (int index, float value);
	Value*				ArraySet (int index, const char* value);
	Value*				ArraySet (int index, const wchar_t* value);
	Value*				ArraySet (int index, const stringc& value);
	Value*				ArraySet (int index, const stringw& value);
	Value*				ArraySet (int index, Value* value);
	Value*				ArraySetMap   (int index);
	Value*				ArraySetArray (int index);
	// Same as ArrayPushBack()->Set(value)
	Value*				ArrayPushBack (bool value);
	Value*				ArrayPushBack (int value);
	Value*				ArrayPushBack (float value);
	Value*				ArrayPushBack (const char* value);
	Value*				ArrayPushBack (const wchar_t* value);
	Value*				ArrayPushBack (const stringc& value);
	Value*				ArrayPushBack (const stringw& value);
	Value*				ArrayPushBack (const SColor& value);
	Value*				ArrayPushBack (Value* value);
	Value*				ArrayPushBackMap   ();
	Value*				ArrayPushBackArray ();
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// XML reading/writing.
	static Value*		Read  (const stringc& file);
	void				Write (const stringc& file);
	// ------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------------------
	// Only use these if you know what you're doing. Prefer Read/Write.
	void				DoRead  (IXMLReader* reader, stringc* key, bool doRead);
	void				DoWrite (IXMLWriter* writer, const wchar_t* key);
	// ------------------------------------------------------------------------------------------------------

private:
	// Crashes if the type is different than m_type.
	void				CheckType (ValueType type);

	ValueType m_type;
	union
	{
		bool m_bool;
		int m_int;
		float m_float;
		stringw* m_string;
		map<stringc, Value*>* m_map;
		array<Value*>* m_array;
	};
};
