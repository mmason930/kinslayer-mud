//Fogel
//April 2010
/**********************************************************
 * Input: a SORTED array "arr", a "key" to search for,    *
 *        and the positions of the array's first and last *
 *        elements "low" and "high"                       *        
 * Output: -1 if the key is not found. If the key is a    *
 *         number and there is a match the key is         *
 *         returned. If the array contains strings it     *
 *         returns index + 1 if there is a match,         *
 *         otherwise -1.                                  *
 **********************************************************/
function binarySearch( arr, key, fGetKey, low, high ) {
	if( !high && high != 0 )
		high = arr.length-1;
	if( !low )
		low = 0;
	if (high < low)
		return -1;
	
	var mid = Math.floor(low + ((high - low) / 2));
	if( ( fGetKey ? fGetKey(arr[mid]) > key : arr[mid] > key ) )
		return binarySearch( arr, key, fGetKey, low, mid - 1 );
	else if( ( fGetKey ? fGetKey(arr[mid]) < key : arr[mid] < key ) )
		return binarySearch( arr, key, fGetKey, mid + 1, high );
	else
		return mid;
}
//Galnor
//August 07, 2010
/**********************************************************************
 * Input: arr    :an already sorted array. The array must be sorted   *
 *				  sorted according to fGetKey, or, if fGetKey is not  *
 *				  passed, by the value of the elements themselves.    *
 *		  element:The object getting inserted into the array.         *
 *		  fGetKey:Optional. If this is specified, it is a function    *
 *				  that takes in element as an argument. It will return*
 *				  the value of the property the array is sorted by.   *
 * Output: None                                                       *
 **********************************************************************/
function sortedInsert( arr, element, fGetKey ) {
	
	var low, high, mid;
	low = 0;
	high = arr.length-1;
	
	if( !fGetKey )
		fGetKey = function(e){return e;};
	
	while( true ) {
		mid = Math.floor((low+high) / 2);
		
		if( high <= low ) {//Okay, this value is not in the array. Let's find the index where it should be inserted.
	
			mid = Math.max(0, mid);
			if( mid >= arr.length )
				arr.push( element );
			else {
				if( fGetKey(element) > fGetKey(arr[mid])  )
					mid = mid+1;
				arr.splice(mid,0,element);
			}
			break;
		}
		else if( fGetKey(arr[mid]) > fGetKey(element) )
			high = mid - 1;
		else if( fGetKey(arr[mid]) < fGetKey(element) )
			low = mid + 1;
		else {//Value found. Insert here.
			arr.splice(mid,0,element);
			break;
		}
	}
}


