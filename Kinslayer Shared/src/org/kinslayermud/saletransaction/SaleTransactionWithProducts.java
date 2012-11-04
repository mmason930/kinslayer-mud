package org.kinslayermud.saletransaction;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

public class SaleTransactionWithProducts {

  protected SaleTransaction saleTransaction;
  protected List<SaleTransactionProduct> saleTransactionProducts = new LinkedList<SaleTransactionProduct>();
  
  public SaleTransaction getSaleTransaction() {
    
    return saleTransaction;
  }
  
  public void setSaleTransaction(SaleTransaction saleTransaction) {
    
    this.saleTransaction = saleTransaction;
  }
  
  public List<SaleTransactionProduct> getSaleTransactionProducts() {
    
    return saleTransactionProducts;
  }
  
  public void setSaleTransactionProducts(List<SaleTransactionProduct> saleTransactionProducts) {
    
    this.saleTransactionProducts = saleTransactionProducts;
  }
  
  public Set<Integer> getProductIdSet() {
    
    Set<Integer> productIdSet = new HashSet<Integer>();;
    
    for(SaleTransactionProduct saleTransactionProduct : saleTransactionProducts) {
      
      productIdSet.add(saleTransactionProduct.getProductId());
    }
    
    return productIdSet;
  }
  
  public Set<Integer> getSellerUserIdSet() {
    
    Set<Integer> sellerUserIdSet = new HashSet<Integer>();;
    
    for(SaleTransactionProduct saleTransactionProduct : saleTransactionProducts) {
      
      sellerUserIdSet.add(saleTransactionProduct.getSellerUserId());
    }
    
    return sellerUserIdSet;
  }
}
