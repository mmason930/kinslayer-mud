<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
String leftNavSelected = (String)request.getAttribute("LeftNavSelected");
%>
		<div class="leftNav">
			<a class="logoText" href="./">Kinslayer MUD</a>
			<div class="leftNavMenu">
				<ul>
				<!--
					<li><a <%=leftNavSelected.equals("GettingStarted") ? "class='selected'" : "" %> href="#">Getting Started</a></li>
				-->
					<li><a <%=leftNavSelected.equals("Characters") ? "class='selected'" : "" %> href="./classes-and-races">Classes & Races</a></li>
					<!--
					<li><a <%=leftNavSelected.equals("Trades") ? "class='selected'" : "" %> href="#">Trades</a></li>
					-->
					<li><a <%=leftNavSelected.equals("Equipment") ? "class='selected'" : "" %> href="<%=WebSiteUrlUtil.getEquipmentListingUrl(webSupport.getInstanceDomain(), null, null, null) %>">Equipment</a></li>
					<li><a <%=leftNavSelected.equals("SuperMobs") ? "class='selected'" : "" %> href="./supermob-listing">SuperMOBs</a></li>
					<li><a href="./forums">Community</a></li>
					<li><a <%=leftNavSelected.equals("About") ? "class='selected'" : "" %> href="./about-kinslayermud">What is KinslayerMUD?</a></li>
				</ul>
			</div>
		</div>