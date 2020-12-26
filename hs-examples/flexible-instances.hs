{-# LANGUAGE FlexibleInstances #-}

-- | allows the variable on the LHS to be instantiated on the RHS. Needs us
-- to be able to match things like: 
-- Tuple("Monad", m) => Tuple("Foo", Tuple(m, a)) 
-- which is bad, because now the head of a tuple is a variable `m`.
--
-- We can circumvent this by recording things like `m a` as :
class Foo m
instance Monad m => Foo (m a)
