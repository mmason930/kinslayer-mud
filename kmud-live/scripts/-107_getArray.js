function  getArray(row,col)
{
	var a = new Array(row);
	for(var i = 0;i < row;++i)
		a[ i ] = new Array(col);
	return a;
}
