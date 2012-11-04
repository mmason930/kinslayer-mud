package org.kinslayermud.product;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;


public class BookOperator extends BaseProductOperator {

  public Product getProduct(ResultSet resultSet) throws SQLException {

    Book book = new Book();
    
    fillInBaseProperties(resultSet, book);
    
    book.setAuthors(QueryUtil.buildListFromString(resultSet.getString("book.authors")));
    book.setBinding(resultSet.getString("book.binding"));
    book.setIsbn(resultSet.getString("book.isbn"));
    book.setFormats(QueryUtil.buildListFromString(resultSet.getString("book.formats")));
    book.setLanguages(QueryUtil.buildListFromString(resultSet.getString("book.languages")));
    book.setPublisher(resultSet.getString("book.publisher"));
    book.setNumberOfPages(QueryUtil.getInteger(resultSet, "book.number_of_pages"));
    book.setPublicationDate(resultSet.getTimestamp("book.publication_date"));
    
    return book;
  }

  public void putProduct(Statement statement, Product product) throws SQLException {
    
    Book book = (Book)product;
    boolean isNew = book.isNew();
    String sql;
    putBaseProduct(statement, book);
    
    if(isNew) {
      
      sql = " INSERT INTO book("
          + "   `product_id`,"
          + "   `authors`,"
          + "   `publisher`,"
          + "   `isbn`,"
          + "   `number_of_pages`,"
          + "   `publication_date`,"
          + "   `binding`,"
          + "   `languages`,"
          + "   `formats`"
          + " ) VALUES( "
          + book.getId() + ","
          + SQLUtil.escapeQuoteString(QueryUtil.buildStringFromList(book.getAuthors())) + ","
          + SQLUtil.escapeQuoteString(book.getPublisher()) + ","
          + SQLUtil.escapeQuoteString(book.getIsbn()) + ","
          + book.getNumberOfPages() + ","
          + SQLUtil.encodeQuoteDate(book.getPublicationDate()) + ","
          + SQLUtil.escapeQuoteString(book.getBinding()) + ","
          + SQLUtil.escapeQuoteString(QueryUtil.buildStringFromList(book.getLanguages())) + ","
          + SQLUtil.escapeQuoteString(QueryUtil.buildStringFromList(book.getFormats())) + ")";
      
      statement.executeUpdate(sql);
    }
    else {
      
      sql = " UPDATE book SET"
          + "   authors = " + QueryUtil.buildStringFromList(book.getAuthors()) + ","
          + "   publisher = " + SQLUtil.escapeQuoteString(book.getPublisher()) + ","
          + "   isbn = " + SQLUtil.escapeQuoteString(book.getIsbn()) + ","
          + "   number_of_pages = " + book.getNumberOfPages() + ","
          + "   publication_date = " + SQLUtil.encodeQuoteDate(book.getPublicationDate()) + ","
          + "   binding = " + SQLUtil.escapeQuoteString(book.getBinding()) + ","
          + "   languages = " + SQLUtil.escapeQuoteString(QueryUtil.buildStringFromList(book.getLanguages())) + ","
          + "   formats = " + SQLUtil.escapeQuoteString(QueryUtil.buildStringFromList(book.getFormats()))
          + " WHERE product_id = " + book.getId();
      
      statement.executeQuery(sql);
    }
  }
}
