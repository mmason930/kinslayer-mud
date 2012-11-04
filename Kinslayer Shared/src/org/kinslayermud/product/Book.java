package org.kinslayermud.product;

import java.util.Date;
import java.util.List;

public class Book extends Product {
  
  private List<String> authors;
  private String publisher;
  private String isbn;
  private Integer numberOfPages;
  private Date publicationDate;
  private String binding;
  private List<String> languages;
  private List<String> formats;
  
  public Book() {
    
    id = NEW;
  }
  
  public List<String> getAuthors() {
    
    return authors;
  }
  
  public void setAuthors(List<String> authors) {
    
    this.authors = authors;
  }
  
  public String getPublisher() {
    
    return publisher;
  }
  
  public void setPublisher(String publisher) {
    
    this.publisher = publisher;
  }
  
  public String getIsbn() {
    
    return isbn;
  }
  
  public void setIsbn(String isbn) {
    
    this.isbn = isbn;
  }
  
  public Integer getNumberOfPages() {
    
    return numberOfPages;
  }
  
  public void setNumberOfPages(Integer numberOfPages) {
    
    this.numberOfPages = numberOfPages;
  }
  
  public Date getPublicationDate() {
    
    return publicationDate;
  }
  
  public void setPublicationDate(Date publicationDate) {
    
    this.publicationDate = publicationDate;
  }
  
  public String getBinding() {
    
    return binding;
  }
  
  public void setBinding(String binding) {
    
    this.binding = binding;
  }
  
  public List<String> getLanguages() {
    
    return languages;
  }
  
  public void setLanguages(List<String> languages) {
    
    this.languages = languages;
  }
  
  public List<String> getFormats() {
    
    return formats;
  }
  
  public void setFormats(List<String> formats) {
    
    this.formats = formats;
  }
  
  public ProductType getType() {
    
    return ProductType.book;
  }
}
