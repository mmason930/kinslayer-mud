package org.kinslayermud.cart;

import java.math.BigDecimal;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class CartWithProducts {

  protected Cart cart;
  protected List<CartSaleProduct> cartSaleProducts;
  
  public Cart getCart() {
    
    return cart;
  }
  
  public void setCart(Cart cart) {
    
    this.cart = cart;
  }
  
  public List<CartSaleProduct> getCartSaleProducts() {
    
    return cartSaleProducts;
  }
  
  public void setCartSaleProducts(List<CartSaleProduct> cartSaleProducts) {
    
    this.cartSaleProducts = cartSaleProducts;
  }
  
  public Set<Integer> getProductIdSet() {
    
    Set<Integer> productIdSet = new HashSet<Integer>();
    for(CartSaleProduct cartSaleProduct : cartSaleProducts) {
      
      productIdSet.add(cartSaleProduct.getProductId());
    }
    
    return productIdSet;
  }
  
  public Set<Integer> getSellerUserIdSet() {
    
    Set<Integer> sellerUserIdSet = new HashSet<Integer>();
    
    for(CartSaleProduct cartSaleProduct : cartSaleProducts) {
      
      sellerUserIdSet.add(cartSaleProduct.getSellerUserId());
    }
    
    return sellerUserIdSet;
  }
  
  public BigDecimal getTotalAmount() {
    
    BigDecimal total = BigDecimal.ZERO;
    for(CartSaleProduct cartSaleProduct : cartSaleProducts) {
     
      total = total.add(cartSaleProduct.getPrice())
                   .add(cartSaleProduct.getCityTaxAmount())
                   .add(cartSaleProduct.getCountyTaxAmount())
                   .add(cartSaleProduct.getStateTaxAmount())
                   .add(cartSaleProduct.getSpecialTaxAmount())
                   .add(cartSaleProduct.getShippingAmount())
                   .multiply(BigDecimal.valueOf(cartSaleProduct.getQuantity()));
    }
    
    return total;
  }
  
}
