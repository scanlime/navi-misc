<?xml version="1.0" ?>

<!--
  XML Stylesheet for formatting kenpo stuff
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- ================================ Document Root -->
  <xsl:template match="/concept|/technique|/form|/set">
    <html>
      <head>
        <title>
          <xsl:if test="/concept">Concept: </xsl:if>
          <xsl:if test="/technique">Technique: </xsl:if>
          <xsl:if test="/form">Form: </xsl:if>
          <xsl:if test="/set">Set: </xsl:if>
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
              <xsl:if test="/form">forms</xsl:if>
              <xsl:if test="/set">sets</xsl:if>
            </span>
            <div class="section">
              <div class="sectionTop"/>
              <div class="row">
                <ul>
                  <xsl:if test="/concept"><xsl:apply-templates select="document('concepts.xml')//concepts/concept"/></xsl:if>
                  <xsl:if test="/technique">
                    <span class="emph">punch</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='punch']"/>
                    <span class="emph">push</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='push']"/>
                    <span class="emph">kick</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='kick']"/>
                    <span class="emph">grabs and tackles</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='grabs-and-tackles']"/>
                    <span class="emph">hugs and holds</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='hugs-and-holds']"/>
                    <span class="emph">chokes and locks</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='chokes-and-locks']"/>
                    <span class="emph">weapons</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='weapons']"/>
                    <span class="emph">multiple attacks</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='multiple-attacks']"/>
                    <span class="emph">combinations</span>
                    <xsl:apply-templates select="document('techniques.xml')//techniques/technique[@category='combinations']"/>
                  </xsl:if>
                  <xsl:if test="/form"><xsl:apply-templates select="document('forms.xml')//forms/form"/></xsl:if>
                  <xsl:if test="/set">
                    <span class="emph">yellow</span>
                    <xsl:apply-templates select="document('sets.xml')//sets/set[@level='yellow']"/>
                    <span class="emph">orange</span>
                    <xsl:apply-templates select="document('sets.xml')//sets/set[@level='orange']"/>
                    <span class="emph">purple</span>
                    <xsl:apply-templates select="document('sets.xml')//sets/set[@level='purple']"/>
                  </xsl:if>
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
            <xsl:if test="/form">
              <xsl:apply-templates select="steps"/>
              <xsl:apply-templates select="contents"/>
              <xsl:apply-templates select="lessons"/>
              <xsl:apply-templates select="notes"/>
            </xsl:if>
            <xsl:if test="/set">
              <xsl:apply-templates select="steps"/>
            </xsl:if>
          </td>

          <!-- ==== Random Stuff -->
          <xsl:if test="//origin|//attack|/technique//stances|/technique@level">
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
        <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
        <xsl:value-of select="@name"/>
      </a>
      <ul>
        <xsl:apply-templates select="concept"/>
      </ul>
    </li>
  </xsl:template>
  <xsl:template match="//techniques/technique">
    <li>
      <a>
        <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
        <xsl:value-of select="@name"/>
      </a>
    </li>
  </xsl:template>
  <xsl:template match="//forms/form">
    <li>
      <a>
        <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
        <xsl:value-of select="@name"/>
      </a>
    </li>
  </xsl:template>
  <xsl:template match="//sets/set">
    <li>
      <a>
        <xsl:attribute name="href"><xsl:value-of select="@href"/></xsl:attribute>
        <xsl:value-of select="@name"/>
      </a>
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
            <xsl:value-of select="text()"/>
            <xsl:apply-templates select="note"/>
          </div>
        </xsl:if>
        <xsl:if test="position() = last()">
          <div class="row">
            <xsl:value-of select="text()"/>
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
          <xsl:apply-templates/>
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

  <xsl:template match="contents">
    <span class="section">contains</span>
    <div class="section">
      <div class="sectionTop"/>
      <xsl:if test="overview">
        <div class="row">
          <span class="emph">overview: </span>
          <xsl:value-of select="overview"/>
        </div>
      </xsl:if>
      <xsl:if test="stances">
        <div class="row">
          <span class="emph">stances</span>
          <ul>
            <xsl:for-each select="stances/stance">
              <li><xsl:value-of select="."/></li>
            </xsl:for-each>
          </ul>
        </div>
      </xsl:if>
      <xsl:if test="blocks">
        <div class="row">
          <span class="emph">blocks</span>
          <ul>
            <xsl:for-each select="blocks/block">
              <li><xsl:value-of select="."/></li>
            </xsl:for-each>
          </ul>
        </div>
      </xsl:if>
    </div>
  </xsl:template>

  <xsl:template match="lessons">
    <span class="section">lessons</span>
    <div class="section">
      <div class="sectionTop"/>
      <div class="row">
        <ul>
          <xsl:for-each select="lesson">
            <li><xsl:value-of select="."/></li>
          </xsl:for-each>
        </ul>
      </div>
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
        <xsl:apply-templates select="attacker/stance"/>
      </div>
    </xml:if>
    <xml:if test="defender">
      <div class="row">
        <span class="emph">Defender's Stance: </span>
        <xsl:apply-templates select="defender/stance"/>
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

  <xsl:template match="note">
    <div class="row">
      <span class="emph">Note: </span>
      <xsl:value-of select="."/>
    </div>
  </xsl:template>

  <xsl:template match="stance|strike">
    <xsl:value-of select="@side"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="."/>
  </xsl:template>
  <xsl:template match="block">
    <xsl:value-of select="@side"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="."/>
    block
  </xsl:template>
  <xsl:template match="parry">
    <xsl:value-of select="@side"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="."/>
    parry
  </xsl:template>
  <xsl:template match="kick">
    <xsl:value-of select="@side"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="."/>
    kick
  </xsl:template>
  <xsl:template match="knee">
    <xsl:value-of select="@side"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="."/>
    knee
  </xsl:template>
</xsl:stylesheet>
