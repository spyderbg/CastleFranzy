#include "stdafx.h"
#include "CBatchListCompileData.h"

namespace irr
{
namespace video
{

CBatchListCompileData::~CBatchListCompileData()
{
}

E_COMPILE_DATA_TYPE CBatchListCompileData::getType() const
{
	return ECDT_BATCH_LIST;
}

} // end namespace video
} // end namespace irr
