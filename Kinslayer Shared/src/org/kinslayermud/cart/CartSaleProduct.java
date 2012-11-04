package org.kinslayermud.cart;

import java.math.BigDecimal;
import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.shipping.ShippingType;

public class CartSaleProduct extends DataObjectWithIntId {

  protected int cartId;
  protected int saleListingId;
  protected int sellerUserId;
  protected int productId;
  protected Date createdDatetime;
  protected BigDecimal price;
  protected ShippingType shippingType;
  protected BigDecimal shippingAmount;
  protected short quantity;
  protected BigDecimal countyTaxRate;
  protected BigDecimal countyTaxAmount;
  protected BigDecimal cityTaxRate;
  protected BigDecimal cityTaxAmount;
  protected BigDecimal stateTaxRate;
  protected BigDecimal stateTaxAmount;
  protected BigDecimal specialTaxRate;
  protected BigDecimal specialTaxAmount;
  
  public int getCartId() {
    
    return cartId;
  }
  
  public void setCartId(int cartId) {
    
    this.cartId = cartId;
  }
  
  public int getSaleListingId() {
    
    return saleListingId;
  }
  
  public void setSaleListingId(int saleListingId) {
    
    this.saleListingId = saleListingId;
  }

  public int getSellerUserId() {
    
    return sellerUserId;
  }

  public void setSellerUserId(int sellerUserId) {
    
    this.sellerUserId = sellerUserId;
  }

  public int getProductId() {
    
    return productId;
  }

  public void setProductId(int productId) {
    
    this.productId = productId;
  }

  public Date getCreatedDatetime() {
    
    return createdDatetime;
  }

  public void setCreatedDatetime(Date createdDatetime) {
    
    this.createdDatetime = createdDatetime;
  }

  public BigDecimal getPrice() {
    
    return price;
  }

  public void setPrice(BigDecimal price) {
    
    this.price = price;
  }

  public ShippingType getShippingType() {
    
    return shippingType;
  }

  public void setShippingType(ShippingType shippingType) {
    
    this.shippingType = shippingType;
  }

  public BigDecimal getShippingAmount() {
    
    return shippingAmount;
  }

  public void setShippingAmount(BigDecimal shippingAmount) {
    
    this.shippingAmount = shippingAmount;
  }

  public short getQuantity() {
    
    return quantity;
  }

  public void setQuantity(short quantity) {
    
    this.quantity = quantity;
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
}
