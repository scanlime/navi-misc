<?xml version="1.0" ?>
<!--
   XML stylesheet for formatting the navi-misc web site
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!--================================== Document Root -->

  <xsl:template match="/page">
    <html>
      <head>
        <title><xsl:value-of select="@title"/></title>
        <style type="text/css" media="all"> @import url(style.css);</style>
      </head>
      <body>
        <!--========== Heading -->
        <div class="heading">
          <div class="siteName">navi-misc</div>
          <div class="title"><xsl:value-of select="@title"/></div>
          <div class="subtitle"><xsl:value-of select="@subtitle"/></div>
	  <div class="headingTabs">
            <xsl:apply-templates select="document('tabs.xml')//headingTab"/>
	  </div>
        </div>

	<table class="columns">

          <!--========== Navigation -->
	  <td class="left">
            <span class="section">Projects</span>
	    <div class="section">
              <div class="sectionTop"/>
              <div class="row">
                <ul>
		  <xsl:apply-templates select="document('projects.xml')//projects/project"/>
                </ul>
              </div>
            </div>
	  </td>

          <!--========== Main Content -->
	  <td class="main">
            <xsl:apply-templates select="overview"/>
            <xsl:apply-templates select="projectIndex"/>
	  </td>

	  <!--========== Auxiliary Content -->
	  <td class="right">
            <xsl:apply-templates select="activeProjects"/>
	  </td>

        </table>

        <!--========== Footer -->
        <div class="footer">
          <a href="/">
            <img src="/images/web/navi64.png" width="64" height="39" alt="Navi"/>
          </a>
        </div>

      </body>
    </html>
  </xsl:template>

  <!--================================== Heading Tabs -->

  <xsl:template match="headingTab">
    <a class="headingTab">
      <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
      <xsl:value-of select="@name"/>
    </a>
  </xsl:template>

  <!--================================== Projects -->

  <xsl:template match="project">
    <li>
      <a>
        <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
        <xsl:value-of select="@name"/>
      </a>
      <ul>
        <xsl:apply-templates select="project"/>
      </ul>
    </li>
  </xsl:template>

  <xsl:template match="projectIndex">
    <span class="section">Project Index</span>
    <div class="section">
      <div class="sectionTop"/>
      <div class="row">
        <table>
          <tr>
            <th>Name</th>
            <th>Description</th>
            <th>Status</th>
          </tr>
          <xsl:for-each select="document('projects.xml')//project">
            <xsl:sort select="@name"/>
            <tr>
              <td><a>
                 <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
                 <xsl:value-of select="@name"/>
              </a></td>
              <td><xsl:value-of select="description"/></td>
              <td><xsl:value-of select="status"/></td>
            </tr>
          </xsl:for-each>
        </table>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="activeProjects">
    <span class="section">Active</span>
    <div class="section">
      <div class="sectionTop"/>
      <div class="row">
        <ul>
          <xsl:for-each select="document('projects.xml')//project/active">
            <li><xsl:value-of select="../@name"/></li>
          </xsl:for-each>
	</ul>
      </div>
    </div>
  </xsl:template>

  <!--================================== Content -->

  <xsl:template match="overview">
    <span class="section">Overview</span>
    <div class="section">
      <div class="sectionTop"/>
      <xsl:for-each select="p">
        <div class="row">
          <xsl:value-of select="."/>
        </div>
      </xsl:for-each>
    </div>
  </xsl:template>

</xsl:stylesheet>
