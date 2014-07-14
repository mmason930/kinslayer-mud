function isBitSet( bits, iBitPosition )
{
	return (bits & (1 << iBitPosition)) != 0;
}
function toggleBit( bits, iBitPosition )
{
	return bits ^ (1<<iBitPosition);
}
function setBit( bits, iBitPosition )
{
	return bits | (1<<iBitPosition);
}
function removeBit( bits, iBitPosition )
{
	return bits & ~(1<<iBitPosition);
}
function bitsToString( bits, size )
{
	var sBuffer="";
	for(var i = 0;i < size;++i)
	{
		if( isBitSet( bits, i ) )
			sBuffer = "1" + sBuffer
		else
			sBuffer = "0" + sBuffer;
	}
	return sBuffer;
}
