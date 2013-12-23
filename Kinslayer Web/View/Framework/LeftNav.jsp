<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<%
String leftNavSelected = (String)request.getAttribute("LeftNavSelected");
%>
		<div class="leftNav">
			<a class="logoText" href="./">Kinslayer MUD</a>
			<div class="leftNavMenu">
				<ul>
					<li><a <%=leftNavSelected.equals("Characters") ? "class='selected'" : "" %> href="./classes-and-races">Classes & Races</a></li>
					<li><a <%=leftNavSelected.equals("Equipment") ? "class='selected'" : "" %> href="<%=WebSiteUrlUtil.getEquipmentListingUrl(webSupport.getInstanceDomain(), null, null, null) %>">Equipment</a></li>
					<li><a <%=leftNavSelected.equals("SuperMobs") ? "class='selected'" : "" %> href="./supermob-listing">SuperMOBs</a></li>
					<li><a href="./forums" rel="nofollow">Community</a></li>
					<li><a <%=leftNavSelected.equals("About") ? "class='selected'" : "" %> href="./about-kinslayermud">What is KinslayerMUD?</a></li>
					<li><a href="./mudclient/mudclient.php" rel="nofollow">Play Now!</a></li>
<!--
					<li><a <%=leftNavSelected.equals("Help") ? "class='selected'" : "" %> href="./help">Help</a></li>
-->
				</ul>
			</div>
		</div>