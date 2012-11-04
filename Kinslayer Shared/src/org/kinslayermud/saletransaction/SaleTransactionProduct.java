package org.kinslayermud.saletransaction;

import java.math.BigDecimal;
import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.shipping.Carrier;

public class SaleTransactionProduct extends DataObjectWithIntId {

  protected int saleTransactionId;
  protected int productId;
  protected int sellerUserId;
  protected int inventoryId;
  protected SaleTransactionProductStatus status;
  protected Date statusLastModifiedDatetime;
  protected BigDecimal itemAmount;
  protected BigDecimal shippingAmount;
  protected BigDecimal commissionRate;
  protected BigDecimal itemCommissionAmount;
  protected BigDecimal shippingCommissionAmount;
  protected BigDecimal countyTaxRate;
  protected BigDecimal countyTaxAmount;
  protected BigDecimal cityTaxRate;
  protected BigDecimal cityTaxAmount;
  protected BigDecimal stateTaxRate;
  protected BigDecimal stateTaxAmount;
  protected BigDecimal specialTaxRate;
  protected BigDecimal specialTaxAmount;
  protected Carrier carrier;
  protected String trackingNumber;
  protected Date trackingUploadedDatetime;
  
  public SaleTransactionProduct() {
    
    id = NEW;
  }
  
  public int getSaleTransactionId() {
    return saleTransactionId;
  }
  public void setSaleTransactionId(int saleTransactionId) {
    this.saleTransactionId = saleTransactionId;
  }
  public int getProductId() {
    return productId;
  }
  public void setProductId(int productId) {
    this.productId = productId;
  }
  public int getSellerUserId() {
    return sellerUserId;
  }
  public void setSellerUserId(int sellerUserId) {
    this.sellerUserId = sellerUserId;
  }
  public int getInventoryId() {
    return inventoryId;
  }
  public void setInventoryId(int inventoryId) {
    this.inventoryId = inventoryId;
  }
  public SaleTransactionProductStatus getStatus() {
    return status;
  }
  public void setStatus(SaleTransactionProductStatus status) {
    this.status = status;
  }
  public Date getStatusLastModifiedDatetime() {
    return statusLastModifiedDatetime;
  }
  public void setStatusLastModifiedDatetime(Date statusLastModifiedDatetime) {
    this.statusLastModifiedDatetime = statusLastModifiedDatetime;
  }
  public BigDecimal getItemAmount() {
    return itemAmount;
  }
  public void setItemAmount(BigDecimal itemAmount) {
    this.itemAmount = itemAmount;
  }
  public BigDecimal getShippingAmount() {
    return shippingAmount;
  }
  public void setShippingAmount(BigDecimal shippingAmount) {
    this.shippingAmount = shippingAmount;
  }
  public BigDecimal getCommissionRate() {
    return commissionRate;
  }
  public void setCommissionRate(BigDecimal commissionRate) {
    this.commissionRate = commissionRate;
  }
  public BigDecimal getItemCommissionAmount() {
    return itemCommissionAmount;
  }
  public void setItemCommissionAmount(BigDecimal itemCommissionAmount) {
    this.itemCommissionAmount = itemCommissionAmount;
  }
  public BigDecimal getShippingCommissionAmount() {
    return shippingCommissionAmount;
  }
  public void setShippingCommissionAmount(BigDecimal shippingCommissionAmount) {
    this.shippingCommissionAmount = shippingCommissionAmount;
  }
  public BigDecimal getCountyTaxRate() {
    return countyTaxRate;
  }
  public void setCountyTaxRate(BigDecimal countyTaxRate) {
    this.countyTaxRate = countyTaxRate;
  }
  public BigDecimal getCountyTaxAmount() {
    return countyTaxAmount;
  }
  public void setCountyTaxAmount(BigDecimal countyTaxAmount) {
    this.countyTaxAmount = countyTaxAmount;
  }
  public BigDecimal getCityTaxRate() {
    return cityTaxRate;
  }
  public void setCityTaxRate(BigDecimal cityTaxRate) {
    this.cityTaxRate = cityTaxRate;
  }
  public BigDecimal getCityTaxAmount() {
    return cityTaxAmount;
  }
  public void setCityTaxAmount(BigDecimal cityTaxAmount) {
    this.cityTaxAmount = cityTaxAmount;
  }
  public BigDecimal getStateTaxRate() {
    return stateTaxRate;
  }
  public void setStateTaxRate(BigDecimal stateTaxRate) {
    this.stateTaxRate = stateTaxRate;
  }
  public BigDecimal getStateTaxAmount() {
    return stateTaxAmount;
  }
  public void setStateTaxAmount(BigDecimal stateTaxAmount) {
    this.stateTaxAmount = stateTaxAmount;
  }
  public BigDecimal getSpecialTaxRate() {
    return specialTaxRate;
  }
  public void setSpecialTaxRate(BigDecimal specialTaxRate) {
    this.specialTaxRate = specialTaxRate;
  }
  public BigDecimal getSpecialTaxAmount() {
    return specialTaxAmount;
  }
  public void setSpecialTaxAmount(BigDecimal specialTaxAmount) {
    this.specialTaxAmount = specialTaxAmount;
  }
  public Carrier getCarrier() {
    return carrier;
  }
  public void setCarrier(Carrier carrier) {
    this.carrier = carrier;
  }
  public String getTrackingNumber() {
    return trackingNumber;
  }
  public void setTrackingNumber(String trackingNumber) {
    this.trackingNumber = trackingNumber;
  }
  public Date getTrackingUploadedDatetime() {
    return trackingUploadedDatetime;
  }
  public void setTrackingUploadedDatetime(Date trackingUploadedDatetime) {
    this.trackingUploadedDatetime = trackingUploadedDatetime;
  }
}
