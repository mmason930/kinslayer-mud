package org.kinslayermud.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

public abstract class StringUtil {


  public static final String MYSQL_SEPERATOR_SEQUENCE_ESCAPED = "\\,\\|\\,";
  public static final String MYSQL_STORAGE_SEPERATOR_SEQUENCE = ",|,";
  
  public static final String ISO_88591_1 = "ISO-8859-1";
  public static final String UTF_8 = "UTF-8";
  public static final String UNICODE = "UNICODE";

  public static String[] stop_words = new String[] { "a's", "able", "about", "above", "according", "accordingly", "across", "actually", "after", "afterwards", "again", "against", "ain't", "all", "allow", "allows", "almost", "alone", "along", "already", "also", "although", "always", "am", "among", "amongst", "an", "and", "another", "any", "anybody", "anyhow", "anyone", "anything", "anyway", "anyways", "anywhere", "apart", "appear", "appreciate", "appropriate", "are", "aren't", "around", "as", "aside", "ask", "asking", "associated", "at", "available", "away", "awfully", "be", "became", "because", "become", "becomes", "becoming", "been", "before", "beforehand", "behind", "being", "believe", "below", "beside", "besides", "best", "better", "between", "beyond", "both", "brief", "but", "by", "c'mon", "c's", "came", "can", "can't", "cannot", "cant", "cause", "causes", "certain", "certainly", "changes", "clearly", "co", "com", "come", "comes", "concerning", "consequently", "consider", "considering", "contain", "containing", "contains", "corresponding", "could", "couldn't", "course", "currently", "definitely", "described", "despite", "did", "didn't", "different", "do", "does", "doesn't", "doing", "don't", "done", "down", "downwards", "during", "each", "edu", "eg", "eight", "either", "else", "elsewhere", "enough", "entirely", "especially", "et", "etc", "even", "ever", "every", "everybody", "everyone", "everything", "everywhere", "ex", "exactly", "example", "except", "far", "few", "fifth", "first", "five", "followed", "following", "follows", "for", "former", "formerly", "forth", "four", "from", "further", "furthermore", "get", "gets", "getting", "given", "gives", "go", "goes", "going", "gone", "got", "gotten", "greetings", "had", "hadn't", "happens", "hardly", "has", "hasn't", "have", "haven't", "having", "he", "he's", "hello", "help", "hence", "her", "here", "here's", "hereafter", "hereby", "herein", "hereupon", "hers", "herself", "hi", "him", "himself", "his", "hither", "hopefully", "how", "howbeit", "however", "i'd", "i'll", "i'm", "i've", "ie", "if", "ignored", "immediate", "in", "inasmuch", "inc", "indeed", "indicate", "indicated", "indicates", "inner", "insofar", "instead", "into", "inward", "is", "isn't", "it", "it'd", "it'll", "it's", "its", "itself", "just", "keep", "keeps", "kept", "know", "knows", "known", "last", "lately", "later", "latter", "latterly", "least", "less", "lest", "let", "let's", "like", "liked", "likely", "little", "look", "looking", "looks", "ltd", "mainly", "many", "may", "maybe", "me", "mean", "meanwhile", "merely", "might", "more", "moreover", "most", "mostly", "much", "must", "my", "myself", "name", "namely", "nd", "near", "nearly", "necessary", "need", "needs", "neither", "never", "nevertheless", "new", "next", "nine", "no", "nobody", "non", "none", "noone", "nor", "normally", "not", "nothing", "novel", "now", "nowhere", "obviously", "of", "off", "often", "oh", "ok", "okay", "old", "on", "once", "one", "ones", "only", "onto", "or", "other", "others", "otherwise", "ought", "our", "ours", "ourselves", "out", "outside", "over", "overall", "own", "particular", "particularly", "per", "perhaps", "placed", "please", "plus", "possible", "presumably", "probably", "provides", "que", "quite", "qv", "rather", "rd", "re", "really", "reasonably", "regarding", "regardless", "regards", "relatively", "respectively", "right", "said", "same", "saw", "say", "saying", "says", "second", "secondly", "see", "seeing", "seem", "seemed", "seeming", "seems", "seen", "self", "selves", "sensible", "sent", "serious", "seriously", "seven", "several", "shall", "she", "should", "shouldn't", "since", "six", "so", "some", "somebody", "somehow", "someone", "something", "sometime", "sometimes", "somewhat", "somewhere", "soon", "sorry", "specified", "specify", "specifying", "still", "sub", "such", "sup", "sure", "t's", "take", "taken", "tell", "tends", "th", "than", "thank", "thanks", "thanx", "that", "that's", "thats", "the", "their", "theirs", "them", "themselves", "then", "thence", "there", "there's", "thereafter", "thereby", "therefore", "therein", "theres", "thereupon", "these", "they", "they'd", "they'll", "they're", "they've", "think", "third", "this", "thorough", "thoroughly", "those", "though", "three", "through", "throughout", "thru", "thus", "to", "together", "too", "took", "toward", "towards", "tried", "tries", "truly", "try", "trying", "twice", "two", "un", "under", "unfortunately", "unless", "unlikely", "until", "unto", "up", "upon", "us", "use", "used", "useful", "uses", "using", "usually", "value", "various", "very", "via", "viz", "vs", "want", "wants", "was", "wasn't", "way", "we", "we'd", "we'll", "we're", "we've", "welcome", "well", "went", "were", "weren't", "what", "what's", "whatever", "when", "whence", "whenever", "where", "where's", "whereafter", "whereas", "whereby", "wherein", "whereupon", "wherever", "whether", "which", "while", "whither", "who", "who's", "whoever", "whole", "whom", "whose", "why", "will", "willing", "wish", "with", "within", "without", "won't", "wonder", "would", "would", "wouldn't", "yes", "yet", "you", "you'd", "you'll", "you're", "you've", "your", "yours", "yourself", "yourselves", "zero" };
  
  /** Creates a string of the specified character repeated the specified number of times. */
  public static String makeStringOf (char character, int numRepetitions) {
    StringBuffer buf = new StringBuffer(numRepetitions);
    for (int i = 0; i < numRepetitions; i++)
      buf.append(character);
    return buf.toString();
  }

  /** appends ch onto text until it is lenght charaters long  **/
  public static String appendChar(String text, char ch, int length, boolean isRightAligned ){
    
    if( isRightAligned)
    {
      while(text.length()<length)
      {
        text = ch + text;
      }
    }
    else
    {
      while(text.length()<length)
      {
        text = text + ch;
      }
    }
    return text;
  }
  
  /** appends ch onto the end of the text until it is lenght charaters long **/
  public static String appendChar(String text, char ch, int length){
    return appendChar(text, ch, length, false );
  }
  
  /** appends ' ' onto the end of the text until it is lenght charaters long on the right **/
  public static String appendSpace(String text, int length){
    return appendChar(text, ' ', length, false );
  }
  
  /** appends '0' onto the beginning of the text until it is lenght charaters long **/
  public static String appendZero(String text, int length){
    return appendChar(text, '0', length, true );
  }

  /** Replaces non-ASCII (7-bit) characters, each, with a '?' character. */
  public static String replaceNonASCIICharacters (String source) {
    return replaceNonASCIICharacters(source, "?");
  }

  /**
   * Replaces non-ASCII (7-bit) characters, each, with the specified replacement.<br>
   * <br>
   * Note: replacement can be of any length including the empty string (not null).
   */
  public static String replaceNonASCIICharacters (String source, String replacement) {

    if (source == null)
      return null;

    return source.replaceAll("[^\\p{ASCII}]", replacement);
  }

  public static String RemoveNonPrintableChars(String str) {
    if(str == null)
      return str;
    String newStr = "";
    for(int i = 0; i < str.length(); i++) {
      if( (str.charAt(i) >= ' ' && str.charAt(i) <= '~') || str.charAt(i) == '\t' || str.charAt(i) == '\n')
        newStr += str.charAt(i); 
    }
    return newStr; 
  }

  public static String ConvertHTMLCharsToAscii(String text)
  {
    text = text.replaceAll("&amp;","&");
    text = text.replaceAll("&quot;","\"");
    text = text.replaceAll("&#161;","!");
    text = text.replaceAll("&#162;","");
    text = text.replaceAll("&#163;","");
    text = text.replaceAll("&#164;","");
    text = text.replaceAll("&#165;","");
    text = text.replaceAll("&#166;","");
    text = text.replaceAll("&#167;","");
    text = text.replaceAll("&#168;","");
    text = text.replaceAll("&#169;","");
    text = text.replaceAll("&#170;","");
    text = text.replaceAll("&#171;","");
    text = text.replaceAll("&#171;","");
    text = text.replaceAll("&#172;","");
    text = text.replaceAll("&#173;","");
    text = text.replaceAll("&#174;","");
    text = text.replaceAll("&#175;","");
    text = text.replaceAll("&#176;","");
    text = text.replaceAll("&#177;","");
    text = text.replaceAll("&#178;","");
    text = text.replaceAll("&#179;","");
    text = text.replaceAll("&#180;","");
    text = text.replaceAll("&#181;","");
    text = text.replaceAll("&#182;","");
    text = text.replaceAll("&#183;","");
    text = text.replaceAll("&#184;","");
    text = text.replaceAll("&#185;","");
    text = text.replaceAll("&#186;","");
    text = text.replaceAll("&#187;","");
    text = text.replaceAll("&#188;","");
    text = text.replaceAll("&#189;","");
    text = text.replaceAll("&#190;","");
    text = text.replaceAll("&#191;","?");
    text = text.replaceAll("&#192;","A");
    text = text.replaceAll("&#193;","A");
    text = text.replaceAll("&#194;","A");
    text = text.replaceAll("&#195;","A");
    text = text.replaceAll("&#196;","A");
    text = text.replaceAll("&#197;","A");
    text = text.replaceAll("&#198;","AE");
    text = text.replaceAll("&#199;","C");
    text = text.replaceAll("&#200;","E");
    text = text.replaceAll("&#201;","E");
    text = text.replaceAll("&#202;","E");
    text = text.replaceAll("&#203;","E");
    text = text.replaceAll("&#204;","I");
    text = text.replaceAll("&#205;","I");
    text = text.replaceAll("&#206;","I");
    text = text.replaceAll("&#207;","I");
    text = text.replaceAll("&#208;","D");
    text = text.replaceAll("&#209;","N");
    text = text.replaceAll("&#210;","O");
    text = text.replaceAll("&#211;","O");
    text = text.replaceAll("&#212;","O");
    text = text.replaceAll("&#213;","O");
    text = text.replaceAll("&#214;","O");
    text = text.replaceAll("&#215;","*");
    text = text.replaceAll("&#216;","O");
    text = text.replaceAll("&#217;","U");
    text = text.replaceAll("&#218;","U");
    text = text.replaceAll("&#219;","U");
    text = text.replaceAll("&#220;","U");
    text = text.replaceAll("&#221;","Y");
    text = text.replaceAll("&#222;","P");
    text = text.replaceAll("&#223;","B");
    text = text.replaceAll("&#224;","a");
    text = text.replaceAll("&#225;","a");
    text = text.replaceAll("&#226;","a");
    text = text.replaceAll("&#227;","a");
    text = text.replaceAll("&#228;","a");
    text = text.replaceAll("&#229;","a");
    text = text.replaceAll("&#230;","ae");
    text = text.replaceAll("&#231;","c");
    text = text.replaceAll("&#232;","e");
    text = text.replaceAll("&#233;","e");
    text = text.replaceAll("&#234;","e");
    text = text.replaceAll("&#235;","e");
    text = text.replaceAll("&#236;","i");
    text = text.replaceAll("&#237;","i");
    text = text.replaceAll("&#238;","i");
    text = text.replaceAll("&#239;","i");
    text = text.replaceAll("&#240;","o");
    text = text.replaceAll("&#241;","n");
    text = text.replaceAll("&#242;","o");
    text = text.replaceAll("&#243;","o");
    text = text.replaceAll("&#244;","o");
    text = text.replaceAll("&#245;","o");
    text = text.replaceAll("&#246;","o");
    text = text.replaceAll("&#247;","");
    text = text.replaceAll("&#248;","o");
    text = text.replaceAll("&#249;","u");
    text = text.replaceAll("&#250;","u");
    text = text.replaceAll("&#251;","u");
    text = text.replaceAll("&#252;","u");
    text = text.replaceAll("&#253;","y");
    text = text.replaceAll("&#254;","p");
    text = text.replaceAll("&#255;","y");
    return text; 
  }
  
  public static String ConvertToHTML(String text)
  {
    return ConvertToHTML(text, 8);
  }
  
  public static String ConvertToHTML(String text, int spacesPerTab)
  {
    if(text == null)
      return null;
    
    int SPACES_PER_TAB = spacesPerTab; 
    
    StringBuffer html = new StringBuffer();
    

    int charCount = 0;
    boolean flag = false;  //if the last char was converted to a &nbsp
    
    if(text.length() > 0)
    {
      if(text.charAt(0) == '\n')
      {
        html.append("<br>\n");
        flag = false;
        charCount = 0;
      }
      else if(text.charAt(0) == '\t')
      {
        for(int j = 0; j < charCount % SPACES_PER_TAB; j++)
        {
          html.append("&nbsp");
          charCount++;
        }
        flag = true;
      }
      else
      {
        html.append(text.charAt(0));
        flag = false;
        charCount++;
      }
    }
    if(text.length() > 1)
    {
      char prev = text.charAt(0);
      
      for(int i = 1; i < text.length(); i++)
      {
        if( (flag || prev == ' ') && text.charAt(i) == ' ')
        {
          html.append("&nbsp");
          flag = true;
          charCount++;
        }
        else if(text.charAt(i) == '\n')
        {
          html.append("<br>\n");
          flag = false;
          charCount = 0;
        }
        else if(text.charAt(i) == '<') {
          html.append("&lt;");
        }
        else if(text.charAt(i) == '>') {
          html.append("&gt;");
        }
        else if(text.charAt(i) == '"') {
          html.append("&quot;");
        }
        else if(text.charAt(i) == '\'') {
          html.append("&apos;");
        }
        else if(text.charAt(i) == '\t')
        {
          for(int j = 0; j < charCount % SPACES_PER_TAB; j++)
          {
            html.append("&nbsp");
            charCount++;
          }
          flag = true;
        }
        else
        {
          html.append(text.charAt(i));
          flag = false;
          charCount++;
        }
            
        prev = text.charAt(i);
      }
    }
    
    return html.toString();
  }

  /** returns a empty string if the string is null */
  public static String removeNull (String string) {
    return (string==null)?"":string;
  }
  
  public static String[] removeNull (String[] strings) {
    for(int i = 0; i < strings.length; i ++)
      strings[i] = removeNull(strings[i]);
    return strings;
  }

  /** Returns true if the string is null or made up of only spaces. */
  public static boolean isNullOrEmptyTrimmedString (String value) {
    return (value == null || value.trim().length() == 0);
  }

  public static String prefixToLength (String value, int length, char prefixChar) {
    int numCharsToAdd = length - value.length();
    while (numCharsToAdd > 0) {
      value = prefixChar + value;
      numCharsToAdd--;
    }

    return value;
  }

  public static String rightSubstring (String value, int length) {
    int start = value.length() - length;
    return (start > 0) ? value.substring(start) : value;
  }
  ////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////
  
  public static String truncateText (String text, int maxLength, String endStr) {
    String temptext = StringUtil.removeNull(text);
    maxLength = (maxLength < 3)?3:maxLength;
    return (temptext.length() > maxLength && endStr != null)
      ? temptext.substring(0, maxLength - 3) + endStr
      : temptext;
  }

  /**
   * If the specified text is longer than the specified length, it is truncated
   * and an ellipses is added to the end.  The final length of a truncated string
   * (including ellipses) will be equal to maxLength.
   * if a null String is passed in a empty string is assumed.
   * if maxLength is less then 3 it is set to 3.
   */
  
  public static String truncateTextWithEllipses (String text, int maxLength) {
    return truncateText(text, maxLength, "...");
  }

   
  /**
   * Returns null if the value is the empty string ("") or null.  Otherwise
   * the original string is returned.
   */
  public static String nullIfEmpty (String value) {
    return (value != null && value.length() > 0) ? value : null;
  }

  public static String nullIfEmptyTrimmed (String value) {
    return nullIfEmptyTrimmed(value, false);
  }

  /**
   * Returns null if the value is a string containing only spaces, the empty
   * string (""), or null.  Otherwise the original string possibly trimmed
   * depending on the parameters.
   */
  public static String nullIfEmptyTrimmed (String value, boolean returnTrimmedStringIfNotEmpty) {
    if (value == null)
      return null;

    String trimmedValue = value.trim();
    if (trimmedValue.length() == 0)
      return null;

    return returnTrimmedStringIfNotEmpty ? trimmedValue : value;
  }

  /** returns string with the first char toUppered */
  public static String properString (String string) {
    if(string == null || string.length() == 0)
    return string;
    else if(string.length() == 1)
      return string.toUpperCase();
    else
      return string.substring(0,1).toUpperCase() + string.substring(1,string.length());
  }
   
  /** returns string broken into many lines using endLineChar with Sperator defualted to '-' */
  public static String stringWrap(String string, int lineLength, int maxRows, String endLineChar)
  {
    return stringWrap(string, lineLength, maxRows, endLineChar, "-");
  }

   
  /** returns string broken into many lines using endLineChar */
  public static String stringWrap(String string, int lineLength, int maxRows, String endLineChar, String Sperator)
  {
    if(string == null)
      return null;
    
    String result[] = new String[maxRows];
    maxRows--;
    
    int index = 0;
    result[0] = string;
    while(index < maxRows && result[index].length() > lineLength)
    {
      if(result[index].charAt(lineLength-1) == ' ')
      {
        result[index+1] = result[index].substring(lineLength);
        result[index] = result[index].substring(0,lineLength-1) + endLineChar;
        index++;
      }
      else if(result[index].charAt(lineLength-1) == '.' || 
              result[index].charAt(lineLength-1) == '?' ||
              result[index].charAt(lineLength-1) == ',' ||
              result[index].charAt(lineLength-1) == ';' ||
              result[index].charAt(lineLength-1) == ':' ||
              result[index].charAt(lineLength-1) == ';' ||
              result[index].charAt(lineLength-1) == '!' ||
              result[index].charAt(lineLength-1) == '-' )
      {
        result[index+1] = result[index].substring(lineLength);
        result[index] = result[index].substring(0,lineLength) + endLineChar;
        index++;
      }
      else
      {
        int i;
        for(i = lineLength-1; i >= 0; i--)
          if(result[index].charAt(i) == ' ')
            break;
        if( i > lineLength/3)
        {
          result[index+1] = result[index].substring(i+1);
          result[index] = result[index].substring(0,i) + endLineChar;
          index++;
        }
        else
        {
          result[index+1] = result[index].substring(lineLength-1);
          result[index] = result[index].substring(0,lineLength-1) + Sperator + endLineChar;
          index++;
        }
      }
    }
    if(result[index].length() > lineLength)
      result[index] = result[index].substring(0,lineLength-4) + "...";
    
    String finalResult = "";
    for(int i = 0; i <= index; i++)
      finalResult += result[i];
    return finalResult;
  }
  
  /** removes all &nbsp; from a string */
  public static String RemoveNbsp(String Text)
  {
    String S = Text;
    while(S.indexOf("&nbsp;") != -1)
    {
      S = S.substring(0,S.indexOf("&nbsp;")) + S.substring(S.indexOf("&nbsp;")+6);
    }
    return S;
  }
  
  public static String removeCharFromString(String string, char ch)
  {
    String str = "";
    for(int i = 0; i < string.length(); i++)
      if(string.charAt(i) != ch)
        str += string.charAt(i); 
    return str;
  }
  public static String toLowerSubString(String word, String part)
  {
    int index;
    String replacePart = part.toLowerCase();
    String str = word;
    while( (index = str.indexOf(part)) != -1)
    {
      str = str.substring(0,index) + replacePart + str.substring(index + part.length(), str.length());
    }
    return str;
  }
  
  public static String toUpperSubString(String word, String part)
  {
    int index;
    String replacePart = part.toUpperCase();
    String str = word;
    while( (index = str.indexOf(part)) != -1)
    {
      str = str.substring(0,index) + replacePart + str.substring(index + part.length(), str.length());
    }
    return str;
  }

  public static String removeIllegalChars(String field) {
    if(field != null) {
      field = StringUtil.RemoveNonPrintableChars(field);
      field = field.replace('\t',' ');
      field = field.replace('\n',' ');
      field = field.replace('|',' ');
    }
    return field;
  }
  
  public static String createLine(String [] fields, char seperator)
  {
    String result = fields[0];
    for(int i = 1; i<fields.length; i++)
      result += seperator+fields[i];
    return result;
  }
  
  public static String createLine(List<String> fields, char seperator)
  {
    String result = "";
    Iterator iterator = fields.iterator();
    if(iterator.hasNext())
      result = (String)iterator.next();
    
    while(iterator.hasNext())
      result += seperator + (String)iterator.next();
    return result;
  }
  
  public static String encodeHTMLCharacters (String value) {
    if(value == null)
      return value;
    return value.replaceAll("&", "&amp;")
                .replaceAll("<", "&lt;")
                .replaceAll(">", "&gt;")
                .replaceAll("\"", "&#x22;")
                .replaceAll("\\)", "&#x29;")
                .replaceAll("\\(", "&#x28;");
  }
  
  public static String escapeHTMLCharacters (String value) {
    if(value == null)
      return value;
    return value.replaceAll("&", "&amp;")
                .replaceAll("<", "&lt;")
                .replaceAll(">", "&gt;")
                .replaceAll("\"", "&quot;")
                .replaceAll("'", "&apos;");
  }
  
  public static String replaceSpaceWithNBSP (String value) {
    if(value == null)
      return value;
    
    return replaceExpression(value, " ", "&nbsp;");
  }
  
  public static String replaceExpression (String value, String expression, String replacement) {
    if(value == null)
      return value;
    
    return value.replaceAll(expression, replacement);
  }
  
  public static String buildListString (Collection elements, String seperator) {

    if(elements.isEmpty())
      return null;
    
    StringBuilder strBuf = new StringBuilder("");
    Iterator iterator = elements.iterator();
    boolean firstPass = true;
    while (iterator.hasNext()) {
      if (firstPass)
        firstPass = false;
      else
        strBuf.append(seperator + " ");

      Object element = iterator.next();
      strBuf.append(element);
    }
    
    return strBuf.toString();
  }
  
  public static Set<String> getKeywords (String ... values) {

    //Clean the string to only be letters and spaces
    
    StringBuffer value_strBuf = new StringBuffer("");
    for(String val : values) {
      if(val != null) {
        value_strBuf = value_strBuf.append(val + " ");
      }
    }

    int textLength = value_strBuf.length();
    StringBuffer strBuf = new StringBuffer(textLength);
    for (int i = 0; i < textLength; i++) {
      char ch = value_strBuf.charAt(i);
      if (Character.isLetter(ch)
          || ch == ' '
          || ch == '\'') {
        strBuf.append(ch);
      }
      else {
        strBuf.append(" ");
      }
    }

    String value = strBuf.toString();
    
    String[] keywords_split = value.split(" ");
    
    Set<String> keywords = new HashSet<String>();
    
    for(int i=0; i<keywords_split.length; i++) {
      
      String keyword = keywords_split[i].toLowerCase();
      
      if(keyword.length() < 3) {
        continue;
      }
      
      if(!isStopWord(keyword)) {
        keywords.add(keyword);
      }
    }
    
    return keywords;
  }
  
  public static boolean isStopWord (String value) {
    
    for(int i=0; i<stop_words.length; i++) {
      if(stop_words[i].equals(value.toLowerCase())) {
        return true;
      }
    }
    
    return false;
  }
  
  public static String buildStringFromList (List<String> values, String seperator_sequence) {

    if(values == null || values.size() == 0) {
      return null;
    }
    
    StringBuilder value = new StringBuilder("");

    Iterator<String> iterator = values.iterator();
    while(iterator.hasNext()) {
      String temp_value = (String)iterator.next();

      value.append(temp_value);

      if(iterator.hasNext()) {
        value.append(seperator_sequence);
      }
    }
    
    return value.toString();
  }

  public static List<String> buildListFromString (String value, String seperator_sequence) {

    List<String> values = new ArrayList<String>();
    
    if(value != null) {
      
      String[] temp_values = value.split(seperator_sequence);
      
      for(int i=0; i<temp_values.length; i++) {
        
        values.add(temp_values[i]);
      }
    }
    
    return values;
  }
  
  public static String plural(long number) {
    
    return number == 1 ? "" : "s";
  }
}