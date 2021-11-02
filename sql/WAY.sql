SELECT DISTINCT
/* Запрос по направлениям */
c.UP_NOM    as KOD,
d.NAME      as NAME 		

FROM WAY c , UP d

WHERE 
/* c.up_nom = :P and  d.up_nom = c.up_nom , где P - параметр кода напраления */
c.up_nom = :DIR_CODE and 
d.up_nom = c.up_nom


