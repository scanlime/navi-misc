<?xml version="1.0" ?>

<!--
  XML Stylesheet for formatting kenpo stuff
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- ================================ Document Root -->
  <xsl:template match="/concept|/technique">
    <html>
      <head>
        <title>
          <xsl:if test="/concept">Concept: </xsl:if>
          <xsl:if test="/technique">Technique: </xsl:if>
          <xsl:value-of select="@name"/>
        </title>
        <style type="text/css" media="all"> @import url(style.css);</style>
      </head>
      <body>
        <!-- ==== Heading -->
        <div class="heading">
          <div class="siteName">kenpo-notes</div>
          <div class="title"><xsl:value-of select="@name"/></div>
          <div class="headingTabs">
            <xsl:apply-templates select="document('tabs.xml')//headingTab"/>
          </div>
        </div>

        <table class="columns">
          <!-- ==== Navigation -->
          <td class="left">
            <span class="section">
              <xsl:if test="/concept">concepts</xsl:if>
              <xsl:if test="/technique">techniques</xsl:if>
            </span>
            <div class="section">
              <div class="sectionTop"/>
              <div class="row">
                <ul>
                  <xsl:if test="/concept"><xsl:apply-templates select="document('concepts.xml')//concepts/concept"/></xsl:if>
                  <xsl:if test="/technique"><xsl:apply-templates select="document('techniques.xml')//techniques/technique"/></xsl:if>
                </ul>
              </div>
            </div>
          </td>

          <!-- ==== Main Content -->
          <td class="main">
            <xsl:if test="/concept">
              <xsl:apply-templates select="overview"/>
              <xsl:apply-templates select="techniques"/>
            </xsl:if>
            <xsl:if test="/technique">
              <xsl:apply-templates select="steps"/>
              <xsl:apply-templates select="notes"/>
            </xsl:if>
          </td>

          <!-- ==== Random Stuff -->
          <xsl:if test="//origin|//attack|//stances|/technique@level">
            <td class="right">
              <span class="section">other</span>
              <div class="section">
                <div class="sectionTop"/>
                <div class="row">
                  <xsl:apply-templates select="origin"/>
                  <xsl:apply-templates select="attack"/>
                  <xsl:apply-templates select="@category"/>
                  <xsl:apply-templates select="@level"/>
                  <xsl:apply-templates select="stances"/>
                </div>
              </div>
            </td>
          </xsl:if>
        </table>

        <div class="footer">
          <a href="/">
            <img src="http://navi/images/web/navi64.png" width="64" height="39" alt="Navi"/>
          </a>
        </div>

      </body>
    </html>
  </xsl:template>

  <!-- ================================ Heading Tabs -->
  <xsl:template match="headingTab">
    <a class="headingTab">
      <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
      <xsl:value-of select="@name"/>
    </a>
  </xsl:template>

  <!-- ================================ Navigation -->
  <xsl:template match="//concepts/concept">
    <li>
      <a>
        <xsl:attribute name="href">concepts/<xsl:value-of select="@href"/></xsl:attribute>
        <xsl:value-of select="@name"/>
      </a>
      <ul>
        <xsl:apply-templates select="concept"/>
      </ul>
    </li>
  </xsl:template>

  <!-- ================================ Content -->
  <xsl:template match="overview">
    <span class="section">overview</span>
    <div class="section">
      <div class="sectionTop"/>
      <div class="row">
        <xsl:value-of select="."/>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="techniques">
    <span class="section">techniques</span>
    <div class="section">
      <div class="sectionTop"/>
      <xsl:for-each select="technique">
        <xsl:if test="not(position() = last())">
          <div class="rowMiddle">
            <xsl:value-of select="."/>
            <xsl:apply-templates select="note"/>
          </div>
        </xsl:if>
        <xsl:if test="position() = last()">
          <div class="row">
            <xsl:value-of select="."/>
            <xsl:apply-templates select="note"/>
          </div>
        </xsl:if>
      </xsl:for-each>
    </div>
  </xsl:template>

  <xsl:template match="steps">
    <span class="section">steps</span>
    <div class="section">
      <div class="sectionTop"/>
      <xsl:for-each select="step">
        <div class="row">
          <xsl:number format="1. "/>
          <xsl:value-of select="."/>
        </div>
      </xsl:for-each>
    </div>
  </xsl:template>

  <xsl:template match="notes">
    <span class="section">notes</span>
    <div class="section">
      <div class="sectionTop"/>
      <xsl:for-each select="note">
        <xsl:if test="not(position() = last())">
          <div class="rowMiddle">
            <xsl:value-of select="."/>
          </div>
        </xsl:if>
        <xsl:if test="(position() = last()">
          <div class="row">
            <xsl:value-of select="."/>
          </div>
        </xsl:if>
      </xsl:for-each>
    </div>
  </xsl:template>

  <xsl:template match="origin">
    <div class="row">
      <span class="emph">Origin: </span>
      <xsl:value-of select="."/>
    </div>
  </xsl:template>

  <xsl:template match="attack">
    <div class="row">
      <span class="emph">Attack: </span>
      <xsl:value-of select="."/>
    </div>
  </xsl:template>

  <xsl:template match="stances">
    <xml:if test="attacker">
      <div class="row">
        <span class="emph">Attacker's Stance: </span>
        <xsl:value-of select="attacker"/>
      </div>
    </xml:if>
    <xml:if test="defender">
      <div class="row">
        <span class="emph">Defender's Stance: </span>
        <xsl:value-of select="defender"/>
      </div>
    </xml:if>
  </xsl:template>

  <xsl:template match="@level">
    <div class="row">
      <span class="emph">Parker Level: </span>
      <xsl:value-of select="."/>
    </div>
  </xsl:template>

  <xsl:template match="@category">
    <div class="row">
      <span class="emph">Category: </span>
      <a>
        <xsl:attribute name="href">../<xsl:value-of select="."/>.xml</xsl:attribute>
        <xsl:value-of select="."/>
      </a>
    </div>
  </xsl:template>
</xsl:stylesheet>
