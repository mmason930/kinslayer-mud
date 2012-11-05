<%@ taglib uri="http://tiles.apache.org/tags-tiles" prefix="tiles" %>
<%@ include file="/View/Framework/Kinslayer.jsp" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<meta http-equiv="Content-Language" content="EN" /> 
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8"> 
		<meta http-equiv="content-type" content="text/html;charset=iso-8859-2" />
		<title><tiles:insertAttribute name="title" /></title>
		<base href="http://localhost/newsite/" />
		<link rel="stylesheet" href="./Styles/style.css" type="text/css"/>
	</head>

	<body>
		<div id="content">
			<div id="header">
				<p id="top_links">
					  <a class="quickNav" href="./index.php">Home</a>
					| <a class="quickNav" href="./new-to-muds">Learn How to Play</a>
					| <a class="quickNav" href="./character-classes">Equipment Catalog</a>
					| <a class="quickNav" href="./legends">Top PVP Players</a>
					| <a class="quickNav" href="./forums">Forums</a>
					| <a class="quickNav" href="./wiki/index.php/Main_Page">Wiki</a>
					| <a class="quickNav" href="<?php echo $signInUrl ?>"><?php echo $signInMessage; ?></a>
<!--
					| <a class="quickNav" href="./player-portal-home">Player Portal</a>
-->
					<br />To log into the game, connect to KinslayerMUD.org using port 2222 with your favorite MUD Client, or Telnet.
					<a href="/mudclient/mudclient.php" class="playNow">&gt; PLAY NOW!</a>
				</p>
			</div> <!-- End of Header -->
			<br />	

			<div id="container">
			
				<div class="content_inner">
					<div class="quickNavContainer">
						<div class="quickNavLeft"></div>
						<ul class="quickNav">
							<li class="quickNav">
								<a href="/" class="quickNavAnchor">Home</a>
							</li>
							<li class="quickNav">
								<a href="./new-to-muds" class="quickNavAnchor">Getting Started</a>
							</li>
							<li class="quickNav">
								<a href="./character-classes" class="quickNavAnchor">Characters</a>
							</li>
							<li class="quickNav">
								<a href="./player-trades" class="quickNavAnchor">Trades</a>
							</li>
							<li class="quickNav">
								<a href="./introduction-to-equipment" class="quickNavAnchor">Equipment</a>
							</li>
							<li class="quickNav">
								<a href="./what-is-a-supermob" class="quickNavAnchor">SuperMOBs</a>
							</li>
							<li class="quickNav">
								<a href="/forums" class="quickNavAnchor">Community</a>
							</li>
						</ul>
						<div class="quickNavRight"></div>
						<div class="clearBoth"></div>
						<img src="images/banner-960x140.jpg" style="display:block;"></img>
					</div>
					
				<tiles:insertAttribute name="body" />
				
				</div> <!-- End Content Inner -->
					
			</div>	<!-- End of Container -->
			<div class="footer">
				<p>
					&copy; Copyright <?php echo strftime("%Y", time()); ?> <a href="http://www.kinslayermud.org">KinslayerMUD.org</a>
				</p>
			</div> <!-- End of Footer -->

		</div> <!-- End of Content -->
		<script type="text/javascript">
var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
document.write(unescape("%3Cscript src='" + gaJsHost + "google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E"));
		</script>
		<script type="text/javascript">
try {
var pageTracker = _gat._getTracker("UA-284526-1");
pageTracker._trackPageview();
} catch(err) {}
</script>
	</body>
</html>

