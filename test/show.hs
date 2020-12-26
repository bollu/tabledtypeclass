instance () => (Show, Char);
instance (Show, a) => (Show, (List, a));
demand (Show, (List, Char));
instance (Bounded, a) and (Eq, a) => (Compact, a);
