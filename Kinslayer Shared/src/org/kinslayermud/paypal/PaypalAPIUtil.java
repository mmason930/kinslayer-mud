package org.kinslayermud.paypal;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.math.BigDecimal;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.methods.GetMethod;

import org.kinslayermud.cart.Cart;
import org.kinslayermud.cart.CartWithProducts;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class PaypalAPIUtil {
  
  protected static final String API_USERNAME = "Sortak_1346647179_biz_api1.gmail.com";
  protected static final String API_PASSWORD = "1346647201";
  protected static final String API_VERSION = "63.0";
  protected static final String API_RETURN_URL = "http://dev.mymeatinyourmouth.com/PaypalExpressCheckoutReviewOrder";
  protected static final String API_CANCEL_URL = "http://dev.mymeatinyourmouth.com/ViewCart";
  protected static final String API_SIGNATURE = "AlTft2gSsg.Ps4YrfTeYD0-uQ.pSAj5-t3kW-BcXoxZA-yx9Of2vR9j3";
  protected static final String API_URL = "https://api-3t.sandbox.paypal.com/nvp";
  
  protected static String encodeUrl(String url, Map<String, String> parameterMap) throws UnsupportedEncodingException {
    
    boolean first = true;
    for(String parameterName : parameterMap.keySet()) {
      
      if(first) {
        
        url += "?";
        first = false;
      }
      else {
        
        url += "&";
      }
      
      url += (URLEncoder.encode(parameterName, "UTF-8") + "=" + URLEncoder.encode(parameterMap.get(parameterName), "UTF-8"));
    }
    
    return url;
  }
  
  protected static String submitGet(String url, Map<String, String> parameterMap) throws Exception {
    
    if(parameterMap != null) {
      
      url = encodeUrl(url, parameterMap);
    }
    
    HttpClient client = new HttpClient();
    GetMethod method = new GetMethod(url);

    StringBuffer response = new StringBuffer("");
    method.setRequestHeader("Content-Type","text/xml; charset=utf-8");

    int statusCode = client.executeMethod(method);
    
    if (statusCode != HttpStatus.SC_OK && statusCode != HttpStatus.SC_CREATED) {
      throw new HttpException("Status = " + statusCode);
    }
    
    InputStream rstream = null;
    rstream = method.getResponseBodyAsStream();

    BufferedReader br = new BufferedReader(new InputStreamReader(rstream));
    String line;

    while ((line = br.readLine()) != null) {
      line.trim();
      response = response.append(line);
    }
    br.close();

    return response.toString();
  }
  
  protected static Map<String, String> getParameterMapFromNameValuePairs(String nameValuePairs) throws UnsupportedEncodingException {
    
    Map<String, String> parameterMap = new HashMap<String, String>();
    
    String[] responseParameters = nameValuePairs.split("&");
    for(String parameter : responseParameters) {
      
      int equalsIndex = parameter.indexOf('=');
      
      String parameterName = URLDecoder.decode(parameter.substring(0, equalsIndex), "UTF-8");
      String parameterValue = URLDecoder.decode(parameter.substring(equalsIndex + 1), "UTF-8");
      
      System.out.println(parameterName + " = " + parameterValue);
      parameterMap.put(parameterName, parameterValue);
    }
    
    return parameterMap;
  }
  
  public static void main(String[] args) throws Exception {
    
    //String token = performSetExpressCheckout(BigDecimal.TEN);
    
    //System.out.println("TOKEN: " + token);
    
    String token = "EC-6KP00765Y1129872L";
    PaypalGetExpressCheckoutDetailsResponse response = getExpressCheckoutDetails(token);
    
    System.out.println("Complete");
  }
  
  //Returns the token sent in the Paypal response.
  public static String performSetExpressCheckout(BigDecimal totalAmount) {
    
    try {
      Map<String, String> requestParameterMap = new HashMap<String, String>();
      Map<String, String> responseParameterMap;
      String response;
      
      requestParameterMap.put("USER", API_USERNAME);
      requestParameterMap.put("PWD", API_PASSWORD);
      requestParameterMap.put("VERSION", API_VERSION);
      requestParameterMap.put("RETURNURL", API_RETURN_URL);
      requestParameterMap.put("CANCELURL", API_CANCEL_URL);
      requestParameterMap.put("SIGNATURE", API_SIGNATURE);
      requestParameterMap.put("METHOD", "SetExpressCheckout");
      requestParameterMap.put("PAYMENTREQUEST_0_PAYMENTACTION", "Sale");
      requestParameterMap.put("PAYMENTREQUEST_0_AMT", String.valueOf(totalAmount));
      
      response = submitGet(API_URL, requestParameterMap);
      
      responseParameterMap = getParameterMapFromNameValuePairs(response);
      
      return responseParameterMap.get("TOKEN");
    }
    catch(Throwable throwable) {
      
      throwable.printStackTrace();
      return null;
    }
  }
  
  public static PaypalGetExpressCheckoutDetailsResponse getExpressCheckoutDetails(String token) {
    
    try {
      PaypalGetExpressCheckoutDetailsResponse getExpressCheckoutDetailsResponse = new PaypalGetExpressCheckoutDetailsResponse();
      
      Map<String, String> requestParameterMap = new HashMap<String, String>();
      Map<String, String> responseParameterMap;
      String response;
      
      requestParameterMap.put("USER", API_USERNAME);
      requestParameterMap.put("PWD", API_PASSWORD);
      requestParameterMap.put("SIGNATURE", API_SIGNATURE);
      requestParameterMap.put("VERSION", API_VERSION);
      requestParameterMap.put("TOKEN", token);
      requestParameterMap.put("METHOD", "GetExpressCheckoutDetails");
    
      response = submitGet(API_URL, requestParameterMap);
      
      responseParameterMap = getParameterMapFromNameValuePairs(response);
      
      if(responseParameterMap.get("ACK") == null || !responseParameterMap.get("ACK").equalsIgnoreCase("Success")) {
        
        throw new Exception("ACK has value of " + responseParameterMap.get("ACK"));
      }
      
      getExpressCheckoutDetailsResponse.setAcknowledgement(responseParameterMap.get("ACK"));
      getExpressCheckoutDetailsResponse.setBuild(responseParameterMap.get("BUILD"));
      getExpressCheckoutDetailsResponse.setVersion(responseParameterMap.get("VERSION"));
      getExpressCheckoutDetailsResponse.setToken(responseParameterMap.get("TOKEN"));
      getExpressCheckoutDetailsResponse.setEmailAddress(responseParameterMap.get("EMAIL"));
      getExpressCheckoutDetailsResponse.setCorrelationId(responseParameterMap.get("CORRELATIONID"));
      getExpressCheckoutDetailsResponse.setPayerId(responseParameterMap.get("PAYERID"));
      getExpressCheckoutDetailsResponse.setPayerStatus(responseParameterMap.get("PAYERSTATUS"));
      getExpressCheckoutDetailsResponse.setFirstName(responseParameterMap.get("FIRSTNAME"));
      getExpressCheckoutDetailsResponse.setLastName(responseParameterMap.get("LASTNAME"));
      getExpressCheckoutDetailsResponse.setCountryCode(responseParameterMap.get("COUNTRYCODE"));
      getExpressCheckoutDetailsResponse.setBusinessName(responseParameterMap.get("BUSINESS"));
      getExpressCheckoutDetailsResponse.setShippingName(responseParameterMap.get("SHIPTONAME"));
      getExpressCheckoutDetailsResponse.setShippingStreetAddressLine1(responseParameterMap.get("SHIPTOSTREET"));
      getExpressCheckoutDetailsResponse.setShippingStreetAddressLine2(responseParameterMap.get("SHIPTOSTREET2"));
      getExpressCheckoutDetailsResponse.setShippingCity(responseParameterMap.get("SHIPTOCITY"));
      getExpressCheckoutDetailsResponse.setShippingState(responseParameterMap.get("SHIPTOSTATE"));
      getExpressCheckoutDetailsResponse.setShippingCountryCode(responseParameterMap.get("SHIPTOCOUNTRYCODE"));
      getExpressCheckoutDetailsResponse.setShippingCountryName(responseParameterMap.get("SHIPTOCOUNTRYNAME"));
      getExpressCheckoutDetailsResponse.setShippingPostalCode(responseParameterMap.get("SHIPTOZIP"));
      
      return getExpressCheckoutDetailsResponse;
    }
    catch(Throwable throwable) {
      
      throwable.printStackTrace();
      return null;
    }
  }
  
  public static PaypalDoExpressCheckoutPaymentResponse doExpressCheckoutPayment(CartWithProducts cartWithProducts) throws Exception {
    
    PaypalDoExpressCheckoutPaymentResponse doExpressCheckoutPaymentResponse = new PaypalDoExpressCheckoutPaymentResponse();
    Map<String, String> requestParameterMap = new HashMap<String, String>();
    Map<String, String> responseParameterMap;
    System.out.println("...1");
    Cart cart = cartWithProducts.getCart();
    String response;
    
    System.out.println("Total Amount: " + cartWithProducts.getTotalAmount());
    System.out.println("Payer ID: " + cart.getPayerId());
    System.out.println("Payment Token: " + cart.getPaymentToken());
    
    requestParameterMap.put("USER", API_USERNAME);
    requestParameterMap.put("PWD", API_PASSWORD);
    requestParameterMap.put("SIGNATURE", API_SIGNATURE);
    requestParameterMap.put("VERSION", API_VERSION);
    requestParameterMap.put("PAYMENTREQUEST_0_PAYMENTACTION", "Sale");
    requestParameterMap.put("PAYERID", cart.getPayerId());
    requestParameterMap.put("TOKEN", cart.getPaymentToken());
    requestParameterMap.put("PAYMENTREQUEST_0_AMT", String.valueOf(cartWithProducts.getTotalAmount()));
    requestParameterMap.put("METHOD", "DoExpressCheckoutPayment");
    
    System.out.println("~~~ REQUEST PARAMETERS~~~");
    for(String parameterName : requestParameterMap.keySet()) {
      
      System.out.println(parameterName + " = " + requestParameterMap.get(parameterName));
    }
    System.out.println("~~~~~~~~~~~~~~~~~~~~~~~~~");
    
    System.out.println("...2");
    response = submitGet(API_URL, requestParameterMap);
    
    System.out.println("Response: " + response);
    
    responseParameterMap = getParameterMapFromNameValuePairs(response);
    
    if(responseParameterMap.get("ACK") == null || !responseParameterMap.get("ACK").equalsIgnoreCase("Success")) {
      
      throw new Exception("ACK has value of " + responseParameterMap.get("ACK"));
    }
    
    doExpressCheckoutPaymentResponse.setAcknowledgement(responseParameterMap.get("ACK"));
    doExpressCheckoutPaymentResponse.setVersion(responseParameterMap.get("VERSION"));
    doExpressCheckoutPaymentResponse.setBuild(responseParameterMap.get("BUILD"));
    doExpressCheckoutPaymentResponse.setToken(responseParameterMap.get("VERSION"));
    doExpressCheckoutPaymentResponse.setTransactionId(responseParameterMap.get("PAYMENTINFO_0_TRANSACTIONID"));
    doExpressCheckoutPaymentResponse.setTransactionType(responseParameterMap.get("PAYMENTINFO_0_TRANSACTIONTYPE"));
    doExpressCheckoutPaymentResponse.setPaymentType(responseParameterMap.get("PAYMENTINFO_0_PAYMENTTYPE"));
    doExpressCheckoutPaymentResponse.setAmount(new BigDecimal(responseParameterMap.get("PAYMENTINFO_0_AMT")));
    doExpressCheckoutPaymentResponse.setTaxAmount(new BigDecimal(responseParameterMap.get("PAYMENTINFO_0_TAXAMT")));
    doExpressCheckoutPaymentResponse.setPendingReason(responseParameterMap.get("PAYMENTINFO_0_PENDINGREASON"));
    doExpressCheckoutPaymentResponse.setReasonCode(responseParameterMap.get("PAYMENTINFO_0_REASONCODE"));

    return doExpressCheckoutPaymentResponse;
  }
  
  public static int storeExpressCheckoutPayment(Statement statement, PaypalDoExpressCheckoutPaymentResponse response) throws SQLException {
    
    String sql;
    
    sql = " INSERT INTO paypalExpressCheckoutTransaction("
        + "   `amount`,"
        + "   `tax_amount`,"
        + "   `paypal_express_transaction_id`,"
        + "   `paypal_express_transaction_type`,"
        + "   `paypal_express_payment_type`,"
        + "   `paypal_express_pending_reason`,"
        + "   `paypal_express_reason_code`"
        + " ) VALUES("
        + SQLUtil.putFixedIntegerBigDecimal(response.getAmount()) + ","
        + SQLUtil.putFixedIntegerBigDecimal(response.getTaxAmount()) + ","
        + SQLUtil.escapeQuoteString(response.getTransactionId()) + ","
        + SQLUtil.escapeQuoteString(response.getTransactionType()) + ","
        + SQLUtil.escapeQuoteString(response.getPaymentType()) + ","
        + SQLUtil.escapeQuoteString(response.getPendingReason()) + ","
        + SQLUtil.escapeQuoteString(response.getReasonCode()) + ")";
    
    statement.executeUpdate(sql);
        
    return QueryUtil.getLastInsertedID(statement);
  }
}