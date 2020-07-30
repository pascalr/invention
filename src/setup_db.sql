CREATE TABLE COMPANY(
      ID INT PRIMARY KEY     NOT NULL,
      NAME           TEXT    NOT NULL,
      AGE            INT     NOT NULL,
      ADDRESS        CHAR(50),
      SALARY         REAL );

CREATE TABLE recette(
      NAME           VARCHAR     NOT NULL,
      INSTRUCTIONS   VARCHAR     );

CREATE TABLE aliment(
      ID INT PRIMARY KEY         NOT NULL,
      NAME           VARCHAR     NOT NULL);

CREATE TABLE ingredient(
      NAME           VARCHAR     NOT NULL,
      ALIMENT_ID     INT );
